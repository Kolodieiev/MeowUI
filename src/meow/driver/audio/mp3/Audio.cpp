/*
 * Audio.cpp
 *
 *  Created on: Oct 26.2018
 *
 *  Version 3.0.8q
 *  Updated on: Mar 04.2024
 *      Author: Wolle (schreibfaul1)
 *
 */
#include "Audio.h"
#include "mp3_decoder/mp3_decoder.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
AudioBuffer::AudioBuffer(size_t maxBlockSize)
{
    // if maxBlockSize isn't set use defaultspace (1600 bytes) is enough for aac and mp3 player
    if (maxBlockSize)
        m_resBuffSizeRAM = maxBlockSize;
    if (maxBlockSize)
        m_maxBlockSize = maxBlockSize;
}

AudioBuffer::~AudioBuffer()
{
    if (m_buffer)
        free(m_buffer);
    m_buffer = NULL;
}

void AudioBuffer::setBufsize(int ram, int psram)
{
    if (ram > -1) // -1 == default / no change
        m_buffSizeRAM = ram;
    if (psram > -1)
        m_buffSizePSRAM = psram;
}

int32_t AudioBuffer::getBufsize() { return m_buffSize; }

size_t AudioBuffer::init()
{
    if (m_buffer)
        free(m_buffer);
    m_buffer = NULL;
    if (psramInit() && m_buffSizePSRAM > 0)
    {
        // PSRAM found, AudioBuffer will be allocated in PSRAM
        m_f_psram = true;
        m_buffSize = m_buffSizePSRAM;
        m_buffer = (uint8_t *)ps_calloc(m_buffSize, sizeof(uint8_t));
        m_buffSize = m_buffSizePSRAM - m_resBuffSizePSRAM;
    }
    if (m_buffer == NULL)
    {
        // PSRAM not found, not configured or not enough available
        m_f_psram = false;
        m_buffer = (uint8_t *)heap_caps_calloc(m_buffSizeRAM, sizeof(uint8_t), MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
        m_buffSize = m_buffSizeRAM - m_resBuffSizeRAM;
    }
    if (!m_buffer)
        return 0;
    m_f_init = true;
    resetBuffer();
    return m_buffSize;
}

void AudioBuffer::changeMaxBlockSize(uint16_t mbs)
{
    m_maxBlockSize = mbs;
    return;
}

uint16_t AudioBuffer::getMaxBlockSize() { return m_maxBlockSize; }

size_t AudioBuffer::freeSpace()
{
    if (m_readPtr >= m_writePtr)
    {
        m_freeSpace = (m_readPtr - m_writePtr);
    }
    else
    {
        m_freeSpace = (m_endPtr - m_writePtr) + (m_readPtr - m_buffer);
    }
    if (m_f_start)
        m_freeSpace = m_buffSize;
    return m_freeSpace - 1;
}

size_t AudioBuffer::writeSpace()
{
    if (m_readPtr >= m_writePtr)
    {
        m_writeSpace = (m_readPtr - m_writePtr - 1); // readPtr must not be overtaken
    }
    else
    {
        if (getReadPos() == 0)
            m_writeSpace = (m_endPtr - m_writePtr - 1);
        else
            m_writeSpace = (m_endPtr - m_writePtr);
    }
    if (m_f_start)
        m_writeSpace = m_buffSize - 1;
    return m_writeSpace;
}

size_t AudioBuffer::bufferFilled()
{
    if (m_writePtr >= m_readPtr)
    {
        m_dataLength = (m_writePtr - m_readPtr);
    }
    else
    {
        m_dataLength = (m_endPtr - m_readPtr) + (m_writePtr - m_buffer);
    }
    return m_dataLength;
}

size_t AudioBuffer::getMaxAvailableBytes()
{
    if (m_writePtr >= m_readPtr)
    {
        m_dataLength = (m_writePtr - m_readPtr - 1);
    }
    else
    {
        m_dataLength = (m_endPtr - m_readPtr);
    }
    return m_dataLength;
}

void AudioBuffer::bytesWritten(size_t bw)
{
    m_writePtr += bw;
    if (m_writePtr == m_endPtr)
    {
        m_writePtr = m_buffer;
    }
    if (bw && m_f_start)
        m_f_start = false;
}

void AudioBuffer::bytesWasRead(size_t br)
{
    m_readPtr += br;
    if (m_readPtr >= m_endPtr)
    {
        size_t tmp = m_readPtr - m_endPtr;
        m_readPtr = m_buffer + tmp;
    }
}

uint8_t *AudioBuffer::getWritePtr() { return m_writePtr; }

uint8_t *AudioBuffer::getReadPtr()
{
    size_t len = m_endPtr - m_readPtr;
    if (len < m_maxBlockSize)
    {                                                     // be sure the last frame is completed
        memcpy(m_endPtr, m_buffer, m_maxBlockSize - len); // cpy from m_buffer to m_endPtr with len
    }
    return m_readPtr;
}

void AudioBuffer::resetBuffer()
{
    m_writePtr = m_buffer;
    m_readPtr = m_buffer;
    m_endPtr = m_buffer + m_buffSize;
    m_f_start = true;
    // memset(m_buffer, 0, m_buffSize); //Clear Inputbuffer
}

uint32_t AudioBuffer::getWritePos() { return m_writePtr - m_buffer; }

uint32_t AudioBuffer::getReadPos() { return m_readPtr - m_buffer; }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// clang-format off
Audio::Audio(bool internalDAC /* = false */, uint8_t channelEnabled /* = I2S_SLOT_MODE_STEREO */, uint8_t i2sPort) {

    mutex_audio = xSemaphoreCreateMutex();

#ifdef AUDIO_LOG
    m_f_Log = true;
#endif

#define __malloc_heap_psram(size) \
    heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL)

    m_f_psramFound = psramInit();
    if(m_f_psramFound) m_chbufSize = 4096; else m_chbufSize = 512 + 64;
    if(m_f_psramFound) m_ibuffSize = 4096; else m_ibuffSize = 512 + 64;
    
    m_outBuff = (int16_t*)__malloc_heap_psram(m_outbuffSize);
    m_ibuff = (char*)__malloc_heap_psram(m_ibuffSize);
    m_chbuf = (char*)__malloc_heap_psram(m_chbufSize);

    if(!m_chbuf || !m_outBuff || !m_ibuff) log_e("oom");

    m_f_channelEnabled = channelEnabled;
    m_f_internalDAC = internalDAC;
    m_i2s_num = i2sPort;  // i2s port number

    // -------- I2S configuration -------------------------------------------------------------------------------------------
#if ESP_IDF_VERSION_MAJOR == 5
    m_i2s_chan_cfg.id            = (i2s_port_t)m_i2s_num;  // I2S_NUM_AUTO, I2S_NUM_0, I2S_NUM_1
    m_i2s_chan_cfg.role          = I2S_ROLE_MASTER;        // I2S controller master role, bclk and lrc signal will be set to output
    m_i2s_chan_cfg.dma_desc_num  = 16;                     // number of DMA buffer
    m_i2s_chan_cfg.dma_frame_num = 512;                    // I2S frame number in one DMA buffer.
    m_i2s_chan_cfg.auto_clear    = true;                   // i2s will always send zero automatically if no data to send
    i2s_new_channel(&m_i2s_chan_cfg, &m_i2s_tx_handle, NULL);

    m_i2s_std_cfg.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_16BIT;  // Bits per sample
    m_i2s_std_cfg.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO;   // I2S channel slot bit-width equals to data bit-width
    m_i2s_std_cfg.slot_cfg.slot_mode      = I2S_SLOT_MODE_STEREO;      // I2S_SLOT_MODE_MONO, I2S_SLOT_MODE_STEREO,
    m_i2s_std_cfg.slot_cfg.slot_mask      = I2S_STD_SLOT_BOTH;         // I2S_STD_SLOT_LEFT, I2S_STD_SLOT_RIGHT
    m_i2s_std_cfg.slot_cfg.ws_width       = I2S_DATA_BIT_WIDTH_16BIT;  // WS signal width (i.e. the number of bclk ticks that ws signal is high)
    m_i2s_std_cfg.slot_cfg.ws_pol         = false;                     // WS signal polarity, set true to enable high lever first
    m_i2s_std_cfg.slot_cfg.bit_shift      = true;                      // Set to enable bit shift in Philips mode
    m_i2s_std_cfg.gpio_cfg.bclk           = I2S_GPIO_UNUSED;           // BCLK, Assignment in setPinout()
    m_i2s_std_cfg.gpio_cfg.din            = I2S_GPIO_UNUSED;           // not used
    m_i2s_std_cfg.gpio_cfg.dout           = I2S_GPIO_UNUSED;           // DOUT, Assignment in setPinout()
    m_i2s_std_cfg.gpio_cfg.mclk           = I2S_GPIO_UNUSED;           // MCLK, Assignment in setPinout()
    m_i2s_std_cfg.gpio_cfg.ws             = I2S_GPIO_UNUSED;           // LRC,  Assignment in setPinout()
    m_i2s_std_cfg.gpio_cfg.invert_flags.mclk_inv = false;
    m_i2s_std_cfg.gpio_cfg.invert_flags.bclk_inv = false;
    m_i2s_std_cfg.gpio_cfg.invert_flags.ws_inv   = false;
    m_i2s_std_cfg.clk_cfg.sample_rate_hz = 44100;
    m_i2s_std_cfg.clk_cfg.clk_src        = I2S_CLK_SRC_DEFAULT;        // Select PLL_F160M as the default source clock
    m_i2s_std_cfg.clk_cfg.mclk_multiple  = I2S_MCLK_MULTIPLE_128;      // mclk = sample_rate * 256
    i2s_channel_init_std_mode(m_i2s_tx_handle, &m_i2s_std_cfg);
    I2Sstart(0);
#else
    m_i2s_config.sample_rate          = 16000;
    m_i2s_config.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
    m_i2s_config.channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT;
    m_i2s_config.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1; // interrupt priority
    m_i2s_config.dma_buf_count        = 16;
    m_i2s_config.dma_buf_len          = 512;
    m_i2s_config.use_apll             = 0; // must be disabled in V2.0.1-RC1
    m_i2s_config.tx_desc_auto_clear   = true;   // new in V1.0.1
    m_i2s_config.fixed_mclk           = true;
    m_i2s_config.mclk_multiple        = I2S_MCLK_MULTIPLE_128;

    if (internalDAC)  {
        #ifdef CONFIG_IDF_TARGET_ESP32  // ESP32S3 has no DAC
        printf("internal DAC");
        m_i2s_config.mode             = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN );
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_MSB); // vers >= 2.0.5
        i2s_driver_install((i2s_port_t)m_i2s_num, &m_i2s_config, 0, NULL);
        i2s_set_dac_mode((i2s_dac_mode_t)m_f_channelEnabled);
        if(m_f_channelEnabled != I2S_DAC_CHANNEL_BOTH_EN) {
            m_f_forceMono = true;
        }
        #endif
    }
    else {
        m_i2s_config.mode             = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        m_i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S); // Arduino vers. > 2.0.0
        i2s_driver_install((i2s_port_t)m_i2s_num, &m_i2s_config, 0, NULL);
        m_f_forceMono = false;
    }
    i2s_zero_dma_buffer((i2s_port_t) m_i2s_num);

#endif // ESP_IDF_VERSION_MAJOR == 5
    computeLimit();  // first init, vol = 21, vol_steps = 21
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Audio::~Audio() {
    // I2Sstop(m_i2s_num);
    // InBuff.~AudioBuffer(); #215 the AudioBuffer is automatically destroyed by the destructor
    setDefaults();

#if ESP_IDF_VERSION_MAJOR == 5
    i2s_del_channel(m_i2s_tx_handle);
#else
    i2s_driver_uninstall((i2s_port_t)m_i2s_num); // #215 free I2S buffer
#endif
    if(m_chbuf)       {free(m_chbuf);        m_chbuf        = NULL;}
    if(m_outBuff)     {free(m_outBuff);      m_outBuff      = NULL; }
    if(m_ibuff)       {free(m_ibuff);        m_ibuff        = NULL;}

    vSemaphoreDelete(mutex_audio);
}
// clang-format on
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setBufsize(int rambuf_sz, int psrambuf_sz)
{
    if (InBuff.isInitialized())
    {
        log_e("Audio::setBufsize must not be called after audio is initialized");
        return;
    }
    InBuff.setBufsize(rambuf_sz, psrambuf_sz);
};

void Audio::initInBuff()
{
    if (!InBuff.isInitialized())
    {
        size_t size = InBuff.init();
        if (size > 0)
        {
            log_i("PSRAM %s found, inputBufferSize: %u bytes", InBuff.havePSRAM() ? "" : "not ", size - 1);
        }
    }
    changeMaxBlockSize(1600); // default size mp3 or aac
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
esp_err_t Audio::I2Sstart(uint8_t i2s_num)
{
#if ESP_IDF_VERSION_MAJOR == 5
    return i2s_channel_enable(m_i2s_tx_handle);
#else
    // It is not necessary to call this function after i2s_driver_install() (it is started automatically),
    // however it is necessary to call it after i2s_stop()
    return i2s_start((i2s_port_t)i2s_num);
#endif
}

esp_err_t Audio::I2Sstop(uint8_t i2s_num)
{
#if ESP_IDF_VERSION_MAJOR == 5
    return i2s_channel_disable(m_i2s_tx_handle);
#else
    return i2s_stop((i2s_port_t)i2s_num);
#endif
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setDefaults()
{
    stopSong();
    initInBuff(); // initialize InputBuffer if not already done
    InBuff.resetBuffer();
    MP3Decoder_FreeBuffers();

    m_f_playing = false;
    m_f_ssl = false;
    m_f_metadata = false;
    m_f_tts = false;
    m_f_firstCall = true; // InitSequence for processWebstream and processLokalFile
    m_f_running = false;
    m_f_loop = false;   // Set if audio file should loop
    m_f_unsync = false; // set within ID3 tag but not used
    m_f_exthdr = false; // ID3 extended header

    m_audioCurrentTime = 0; // Reset playtimer
    m_audioFileDuration = 0;
    m_audioDataStart = 0;
    m_audioDataSize = 0;
    m_avr_bitrate = 0;     // the same as m_bitrate if CBR, median if VBR
    m_bitRate = 0;         // Bitrate still unknown
    m_bytesNotDecoded = 0; // counts all not decodable bytes
    m_chunkcount = 0;      // for chunked streams
                           // byteCounter = 0;     // count received data
    m_contentlength = 0;   // If Content-Length is known, count it
    m_curSample = 0;
    m_metaint = 0;        // No metaint yet
    m_controlCounter = 0; // Status within readID3data() and readWaveHeader()
    m_channels = 2;       // assume stereo #209
    m_file_size = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setFileLoop(bool input)
{
    m_f_loop = input;
    return input;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// clang-format off
void Audio::UTF8toASCII(char* str) {

    const uint8_t ascii[60] = {
    //129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148  // UTF8(C3)
    //                Ä    Å    Æ    Ç         É                                       Ñ                  // CHAR
      000, 000, 000, 142, 143, 146, 128, 000, 144, 000, 000, 000, 000, 000, 000, 000, 165, 000, 000, 000, // ASCII
    //149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168
    //      Ö                             Ü              ß    à                   ä    å    æ         è
      000, 153, 000, 000, 000, 000, 000, 154, 000, 000, 225, 133, 000, 000, 000, 132, 134, 145, 000, 138,
    //169, 170, 171, 172. 173. 174. 175, 176, 177, 179, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188
    //      ê    ë    ì         î    ï         ñ    ò         ô         ö              ù         û    ü
      000, 136, 137, 141, 000, 140, 139, 000, 164, 149, 000, 147, 000, 148, 000, 000, 151, 000, 150, 129};

    uint16_t i = 0, j = 0, s = 0;
    bool     f_C3_seen = false;

    while(str[i] != 0) {    // convert UTF8 to ASCII
        if(str[i] == 195) { // C3
            ++i;
            f_C3_seen = true;
            continue;
        }
        str[j] = str[i];
        if(str[j] > 128 && str[j] < 189 && f_C3_seen == true) {
            s = ascii[str[j] - 129];
            if(s != 0) str[j] = s; // found a related ASCII sign
            f_C3_seen = false;
        }
        ++i;
        ++j;
    }
    str[j] = 0;
}
// clang-format on
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::connecttoFS(fs::FS &fs, const char *path, int32_t resumeFilePos)
{

    if (!path)
    { // guard
        log_i("The given path is empty");
        return false;
    }

    xSemaphoreTakeRecursive(mutex_audio, portMAX_DELAY); // #3

    m_resumeFilePos = resumeFilePos;
    setDefaults(); // free buffers an set defaults

    {                                         // open audiofile scope
        const char *audioName = nullptr;      // pointer for the final file path
        char *audioNameAlternative = nullptr; // possible buffer for alternative versions of the audioName

        if (fs.exists(path))
        { // use given path if it exists (issue #86)
            audioName = path;
        }
        else
        { // try alternative path variants
            size_t len = strlen(path);
            size_t copyMemSize = len + 2; // 2 extra bytes for possible leading / and \0 terminator
            size_t copyOffset = 0;

            audioNameAlternative = (char *)__malloc_heap_psram(copyMemSize);
            if (!audioNameAlternative)
            {
                log_e("out of memory");
                xSemaphoreGiveRecursive(mutex_audio);
                return false;
            }

            // make sure the path starts with a /
            if (path[0] != '/')
            {
                audioNameAlternative[0] = '/';
                copyOffset = 1;
            }

            // copy original path to audioNameAlternative buffer
            strncpy(audioNameAlternative + copyOffset, path, copyMemSize - copyOffset);

            if (fs.exists(audioNameAlternative))
            { // use path with extra leading / if it exists
                audioName = audioNameAlternative;
            }
            else
            { // use ASCII version of the path if it exists
                UTF8toASCII(audioNameAlternative);
                if (fs.exists(audioNameAlternative))
                {
                    audioName = audioNameAlternative;
                }
            }
        }

        if (!audioName)
        {
            log_i("File doesn't exist: \"%s\"", path);
            xSemaphoreGiveRecursive(mutex_audio);
            if (audioNameAlternative)
            {
                free(audioNameAlternative);
            }
            return false;
        }

        vTaskDelay(2);

        audiofile = fs.open(audioName);

        // free audioNameAlternative if used
        if (audioNameAlternative)
            free(audioNameAlternative);
    }

    if (!audiofile)
    {
        xSemaphoreGiveRecursive(mutex_audio);
        return false;
    }

    m_file_size = audiofile.size(); // TEST loop

    char *afn = NULL; // audioFileName
    afn = strdup(audiofile.name());

    uint8_t dotPos = lastIndexOf(afn, ".");
    for (uint8_t i = dotPos + 1; i < strlen(afn); ++i)
    {
        afn[i] = toLowerCase(afn[i]);
    }

    if (afn)
    {
        free(afn);
        afn = NULL;
    }

    bool ret = initializeDecoder();
    if (ret)
        m_f_running = true;
    else
        audiofile.close();
    xSemaphoreGiveRecursive(mutex_audio);
    return ret;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::unicode2utf8(char *buff, uint32_t len)
{
    // converts unicode in UTF-8, buff contains the string to be converted up to len
    // range U+1 ... U+FFFF
    uint8_t *tmpbuff = (uint8_t *)malloc(len * 2);
    if (!tmpbuff)
    {
        log_e("out of memory");
        return;
    }
    bool bitorder = false;
    uint16_t j = 0;
    uint16_t k = 0;
    uint16_t m = 0;
    uint8_t uni_h = 0;
    uint8_t uni_l = 0;

    while (m < len - 1)
    {
        if ((buff[m] == 0xFE) && (buff[m + 1] == 0xFF))
        {
            bitorder = true;
            j = m + 2;
        } // LSB/MSB
        if ((buff[m] == 0xFF) && (buff[m + 1] == 0xFE))
        {
            bitorder = false;
            j = m + 2;
        } // MSB/LSB
        ++m;
    } // seek for last bitorder
    m = 0;
    if (j > 0)
    {
        for (k = j; k < len; k += 2)
        {
            if (bitorder == true)
            {
                uni_h = (uint8_t)buff[k];
                uni_l = (uint8_t)buff[k + 1];
            }
            else
            {
                uni_l = (uint8_t)buff[k];
                uni_h = (uint8_t)buff[k + 1];
            }

            uint16_t uni_hl = ((uni_h << 8) | uni_l);

            if (uni_hl < 0X80)
            {
                tmpbuff[m] = uni_l;
                ++m;
            }
            else if (uni_hl < 0X800)
            {
                tmpbuff[m] = ((uni_hl >> 6) | 0XC0);
                ++m;
                tmpbuff[m] = ((uni_hl & 0X3F) | 0X80);
                ++m;
            }
            else
            {
                tmpbuff[m] = ((uni_hl >> 12) | 0XE0);
                ++m;
                tmpbuff[m] = (((uni_hl >> 6) & 0X3F) | 0X80);
                ++m;
                tmpbuff[m] = ((uni_hl & 0X3F) | 0X80);
                ++m;
            }
        }
    }
    memcpy(buff, tmpbuff, m);
    buff[m] = 0;
    if (tmpbuff)
    {
        free(tmpbuff);
        tmpbuff = NULL;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::latinToUTF8(char *buff, size_t bufflen)
{
    // most stations send  strings in UTF-8 but a few sends in latin. To standardize this, all latin strings are
    // converted to UTF-8. If UTF-8 is already present, nothing is done and true is returned.
    // A conversion to UTF-8 extends the string. Therefore it is necessary to know the buffer size. If the converted
    // string does not fit into the buffer, false is returned
    // utf8 bytelength: >=0xF0 3 bytes, >=0xE0 2 bytes, >=0xC0 1 byte, e.g. e293ab is ⓫

    uint16_t pos = 0;
    uint8_t ext_bytes = 0;
    uint16_t len = strlen(buff);
    uint8_t c;

    while (pos < len - 2)
    {
        c = buff[pos];
        if (c >= 0xC2)
        { // is UTF8 char
            pos++;
            if (c >= 0xC0 && buff[pos] < 0x80)
            {
                ext_bytes++;
                pos++;
            }
            if (c >= 0xE0 && buff[pos] < 0x80)
            {
                ext_bytes++;
                pos++;
            }
            if (c >= 0xF0 && buff[pos] < 0x80)
            {
                ext_bytes++;
                pos++;
            }
        }
        else
            pos++;
    }
    if (!ext_bytes)
        return true; // is UTF-8, do nothing

    pos = 0;

    while (buff[pos] != 0)
    {
        if ((buff[pos] & 0x80) == 0)
        {
            pos++;
            continue;
        }
        else
        {
            len = strlen(buff);
            for (int i = len + 1; i > pos; i--)
            {
                buff[i + 1] = buff[i];
            }
            uint8_t c = buff[pos];
            buff[pos] = 0xc0 | ((c >> 6) & 0x1f); // 2+1+5 bits
            pos++;
            buff[pos] = 0x80 | ((char)c & 0x3f); // 1+1+6 bits
        }
        pos++;
        if (pos > bufflen - 3)
        {
            buff[bufflen - 1] = '\0';
            return false; // do not overwrite
        }
    }
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
size_t Audio::readAudioHeader(uint32_t bytes)
{
    size_t bytesReaded = 0;

    int res = read_ID3_Header(InBuff.getReadPtr(), bytes);
    if (res >= 0)
        bytesReaded = res;
    else
    { // error, skip header
        m_controlCounter = 100;
    }

    if (!isRunning())
    {
        log_e("Processing stopped due to invalid audio header");
        return 0;
    }
    return bytesReaded;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::stopSong()
{
    uint32_t pos = 0;
    if (m_f_running)
    {
        m_f_running = false;

        pos = getFilePos() - inBufferFilled();
        audiofile.close();
    }
    if (audiofile)
    {
        // added this before putting 'm_f_localfile = false' in stopSong(); shoulf never occur....
        audiofile.close();
    }
    memset(m_outBuff, 0, m_outbuffSize); // Clear OutputBuffer
    m_validSamples = 0;
    m_audioCurrentTime = 0;
    m_audioFileDuration = 0;
    return pos;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::pauseResume()
{
    xSemaphoreTake(mutex_audio, portMAX_DELAY);
    bool retVal = false;

    m_f_running = !m_f_running;
    retVal = true;
    if (!m_f_running)
    {
        memset(m_outBuff, 0, m_outbuffSize); // Clear OutputBuffer
        m_validSamples = 0;
    }

    xSemaphoreGive(mutex_audio);
    return retVal;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::playChunk()
{

    int16_t sample[2];

    auto pc = [&](int16_t *s16) { // lambda, inner function
        if (playSample(s16))
        {
            m_validSamples--;
            m_curSample++;
            return true;
        }
        return false;
    };

    // If we've got data, try and pump it out..
    while (m_validSamples)
    {

        if (m_channels == 1)
        {
            sample[RIGHTCHANNEL] = m_outBuff[m_curSample];
            sample[LEFTCHANNEL] = m_outBuff[m_curSample];
        }
        else
        {
            if (!m_f_forceMono)
            { // stereo mode
                sample[RIGHTCHANNEL] = m_outBuff[m_curSample * 2];
                sample[LEFTCHANNEL] = m_outBuff[m_curSample * 2 + 1];
            }
            else
            { // mono mode, #100
                int16_t xy = (m_outBuff[m_curSample * 2] + m_outBuff[m_curSample * 2 + 1]) / 2;
                sample[RIGHTCHANNEL] = xy;
                sample[LEFTCHANNEL] = xy;
            }
        }

        if (!pc(sample))
        {
            break;
        } // playSample in lambda
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::loop()
{
    if (!m_f_running)
        return;

    xSemaphoreTake(mutex_audio, portMAX_DELAY);

    processLocalFile();

    xSemaphoreGive(mutex_audio);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::processLocalFile()
{
    if (!(audiofile && m_f_running))
        return; // guard

    static uint32_t ctime = 0;
    const uint32_t timeout = 2500;                          // ms
    const uint32_t maxFrameSize = InBuff.getMaxBlockSize(); // every mp3/aac frame is not bigger
    static bool f_stream;
    static bool f_fileDataComplete;
    static uint32_t byteCounter; // count received data
    uint32_t availableBytes = 0;

    if (m_f_firstCall)
    { // runs only one time per connection, prepare for start
        m_f_firstCall = false;
        f_stream = false;
        f_fileDataComplete = false;
        byteCounter = 0;
        ctime = millis();
        if (m_resumeFilePos == 0)
            m_resumeFilePos = -1; // parkposition
        return;
    }

    availableBytes = 16 * 1024; // set some large value

    availableBytes = min(availableBytes, (uint32_t)InBuff.writeSpace());
    availableBytes = min(availableBytes, audiofile.size() - byteCounter);
    if (m_contentlength)
    {
        if (m_contentlength > getFilePos())
            availableBytes = min(availableBytes, m_contentlength - getFilePos());
    }
    if (m_audioDataSize)
    {
        availableBytes = min(availableBytes, m_audioDataSize + m_audioDataStart - byteCounter);
    }

    int32_t bytesAddedToBuffer = audiofile.read(InBuff.getWritePtr(), availableBytes);

    if (bytesAddedToBuffer > 0)
    {
        byteCounter += bytesAddedToBuffer; // Pull request #42
        InBuff.bytesWritten(bytesAddedToBuffer);
    }
    if (!f_stream)
    {
        if (m_controlCounter != 100)
        {
            if ((millis() - ctime) > timeout)
            {
                log_e("audioHeader reading timeout");
                m_f_running = false;
                return;
            }
            if (InBuff.bufferFilled() > maxFrameSize)
            { // read the file header first
                InBuff.bytesWasRead(readAudioHeader(InBuff.getMaxAvailableBytes()));
            }
            return;
        }
        else
        {
            if ((InBuff.freeSpace() > maxFrameSize) && (m_file_size - byteCounter) > maxFrameSize && availableBytes)
            {
                // fill the buffer before playing
                return;
            }

            f_stream = true;
        }
    }
    if (m_resumeFilePos >= 0)
    {
        if (m_resumeFilePos < m_audioDataStart)
            m_resumeFilePos = m_audioDataStart;
        if (m_resumeFilePos > m_file_size)
            m_resumeFilePos = m_file_size;

        // must be divisible by four
        m_resumeFilePos = mp3_correctResumeFilePos(m_resumeFilePos);

        if (m_avr_bitrate)
            m_audioCurrentTime = ((double)(m_resumeFilePos - m_audioDataStart) / m_avr_bitrate) * 8;
        audiofile.seek(m_resumeFilePos);
        InBuff.resetBuffer();
        byteCounter = m_resumeFilePos;
        f_fileDataComplete = false; // #570

        m_resumeFilePos = -1;
        f_stream = false;
    }
    // end of file reached? - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (f_fileDataComplete && InBuff.bufferFilled() < InBuff.getMaxBlockSize())
    {
        if (InBuff.bufferFilled())
        {
            if (!readID3V1Tag())
            {
                if (m_validSamples)
                {
                    playChunk();
                    return;
                } // play samples first
                int bytesDecoded = sendBytes(InBuff.getReadPtr(), InBuff.bufferFilled());
                if (bytesDecoded <= InBuff.bufferFilled())
                { // avoid InBuff overrun (can be if file is corrupt)
                    if (m_f_playing)
                    {
                        if (bytesDecoded > 2)
                        {
                            InBuff.bytesWasRead(bytesDecoded);
                            return;
                        }
                    }
                }

                log_e("audio file is corrupt --> send EOF"); // no return, fall through
            }
        }

        if (m_f_loop && f_stream)
        {
            setFilePos(m_audioDataStart);
            m_audioCurrentTime = 0;
            byteCounter = m_audioDataStart;
            f_fileDataComplete = false;
            return;
        } // loop

        m_f_running = false;
        audiofile.close();

        MP3Decoder_FreeBuffers();
        return;
    }
    if (byteCounter == audiofile.size())
    {
        f_fileDataComplete = true;
    }
    if (byteCounter == m_audioDataSize + m_audioDataStart)
    {
        f_fileDataComplete = true;
    }
    // play audio data - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (f_stream)
    {
        playAudioData();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::playAudioData()
{
    if (m_validSamples)
    {
        playChunk();
        return;
    } // play samples first
    if (InBuff.bufferFilled() < InBuff.getMaxBlockSize())
        return; // guard

    int bytesDecoded = sendBytes(InBuff.getReadPtr(), InBuff.getMaxBlockSize());

    if (bytesDecoded < 0)
    { // no syncword found or decode error, try next chunk
        log_e("err bytesDecoded %i", bytesDecoded);
        uint8_t next = 200;
        if (InBuff.bufferFilled() < next)
            next = InBuff.bufferFilled();
        InBuff.bytesWasRead(next); // try next chunk
        m_bytesNotDecoded += next;
    }
    else
    {
        if (bytesDecoded > 0)
        {
            InBuff.bytesWasRead(bytesDecoded);
            return;
        }
        if (bytesDecoded == 0)
            return; // syncword at pos0
    }

    return;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::initializeDecoder()
{
    uint32_t gfH = 0;
    uint32_t hWM = 0;

    if (!MP3Decoder_AllocateBuffers())
    {
        log_e("The MP3Decoder could not be initialized");

        stopSong();
        return false;
    }
    gfH = ESP.getFreeHeap();
    hWM = uxTaskGetStackHighWaterMark(NULL);
    InBuff.changeMaxBlockSize(m_frameSizeMP3);

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Audio::findNextSync(uint8_t *data, size_t len)
{
    // Mp3 and aac audio data are divided into frames. At the beginning of each frame there is a sync word.
    // The sync word is 0xFFF. This is followed by information about the structure of the frame.
    // Wav files have no frames
    // Return: 0 the synchronous word was found at position 0
    //         > 0 is the offset to the next sync word
    //         -1 the sync word was not found within the block with the length len

    int nextSync;
    static uint32_t swnf = 0;

    nextSync = MP3FindSyncWord(data, len);

    if (nextSync == -1)
    {
        swnf++; // syncword not found counter, can be multimediadata
    }
    if (nextSync == 0)
    {
        swnf = 0;
    }

    return nextSync;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setDecoderItems()
{
    m_channels = MP3GetChannels();
    setSampleRate(MP3GetSampRate());
    setBitrate(MP3GetBitrate());

    if (m_channels != 1 && m_channels != 2)
    {
        log_e("Num of channels must be 1 or 2, found %i", m_channels);
        stopSong();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int Audio::sendBytes(uint8_t *data, size_t len)
{

    int bytesLeft;
    static bool f_setDecodeParamsOnce = true;
    int nextSync = 0;
    if (!m_f_playing)
    {
        f_setDecodeParamsOnce = true;
        nextSync = findNextSync(data, len);
        if (nextSync == 0)
        {
            m_f_playing = true;
        }
        return nextSync;
    }
    // m_f_playing is true at this pos
    bytesLeft = len;
    m_decodeError = 0;
    int bytesDecoded = 0;

    m_decodeError = MP3Decode(data, &bytesLeft, m_outBuff, 0);

    // m_decodeError - possible values are:
    //                   0: okay, no error
    //                 100: the decoder needs more data
    //                 < 0: there has been an error

    if (m_decodeError < 0)
    { // Error, skip the frame...
      //        i2s_zero_dma_buffer((i2s_port_t)m_i2s_num);
        if (!m_channels && (m_decodeError == -2))
        {
            ; // at the beginning this doesn't have to be a mistake, suppress errorcode MAINDATA_UNDERFLOW
        }
        else
        {
            log_e("Decode error");
            m_f_playing = false; // seek for new syncword
        }
        return 1; // skip one byte and seek for the next sync word
    }
    bytesDecoded = len - bytesLeft;

    if (bytesDecoded == 0 && m_decodeError == 0)
    {                        // unlikely framesize
        m_f_playing = false; // seek for new syncword
        // we're here because there was a wrong sync word so skip one byte and seek for the next
        return 1;
    }
    // status: bytesDecoded > 0 and m_decodeError >= 0
    char *st = NULL;

    m_validSamples = MP3GetOutputSamps() / m_channels;

    if (f_setDecodeParamsOnce)
    {
        f_setDecodeParamsOnce = false;
        setDecoderItems();
        m_PlayingStartTime = millis();
    }

    compute_audioCurrentTime(bytesDecoded);

    m_curSample = 0;
    playChunk();
    return bytesDecoded;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::compute_audioCurrentTime(int bd)
{
    static uint16_t loop_counter = 0;
    static int old_bitrate = 0;
    static uint64_t sum_bitrate = 0;
    static boolean f_CBR = true; // constant bitrate
    static uint8_t cnt = 0;

    setBitrate(MP3GetBitrate());

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_avr_bitrate == 0)
    { // first time
        loop_counter = 1;
        old_bitrate = 0;
        sum_bitrate = 0;
        f_CBR = true;
        m_avr_bitrate = getBitRate();
        old_bitrate = getBitRate();
        cnt = 0;
    }
    if (!getBitRate())
        return;
    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if ((old_bitrate != getBitRate()) && f_CBR)
    {
        f_CBR = false; // variable bitrate
    }
    old_bitrate = getBitRate();

    if (loop_counter < 10000)
    { // then the bit rate is determined with sufficient precision
        sum_bitrate += getBitRate();
        m_avr_bitrate = sum_bitrate / loop_counter;
        loop_counter++;
    }

    m_audioCurrentTime += ((float)bd / m_avr_bitrate) * 8;

    if (cnt == 1)
    {
        m_audioCurrentTime = ((float)(getFilePos() - m_audioDataStart - inBufferFilled()) / m_avr_bitrate) * 8; // #293
    }
    cnt++;
    if (cnt == 100)
        cnt = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setPinout(uint8_t BCLK, uint8_t LRC, uint8_t DOUT, int8_t MCLK)
{
    esp_err_t result = ESP_OK;

#if (ESP_ARDUINO_VERSION_MAJOR < 2)
    log_e("Arduino Version too old!");
#endif
#if (ESP_ARDUINO_VERSION_MAJOR == 2 && ESP_ARDUINO_VERSION_PATCH < 8)
    log_e("Arduino Version must be 2.0.8 or higher!");
#endif

#if (ESP_IDF_VERSION_MAJOR == 5)
    i2s_std_gpio_config_t gpio_cfg = {};
    gpio_cfg.bclk = (gpio_num_t)BCLK;
    gpio_cfg.din = (gpio_num_t)I2S_GPIO_UNUSED;
    gpio_cfg.dout = (gpio_num_t)DOUT;
    gpio_cfg.mclk = (gpio_num_t)MCLK;
    gpio_cfg.ws = (gpio_num_t)LRC;
    I2Sstop(0);
    result = i2s_channel_reconfig_std_gpio(m_i2s_tx_handle, &gpio_cfg);
    I2Sstart(0);
#else
    m_pin_config.bck_io_num = BCLK;
    m_pin_config.ws_io_num = LRC; //  wclk = lrc
    m_pin_config.data_out_num = DOUT;
    m_pin_config.data_in_num = I2S_GPIO_UNUSED;
    m_pin_config.mck_io_num = MCLK;
    result = i2s_set_pin((i2s_port_t)m_i2s_num, &m_pin_config);
#endif
    return (result == ESP_OK);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getFileSize()
{
    if (!audiofile)
    {
        if (m_contentlength > 0)
        {
            return m_contentlength;
        }
        else
        {
            return 0;
        }
    }
    return audiofile.size();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getFilePos()
{
    if (!audiofile)
        return 0;
    return audiofile.position();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getAudioDataStartPos()
{
    if (!audiofile)
        return 0;
    return m_audioDataStart;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getAudioFileDuration()
{
    if (!audiofile)
        return 0;

    if (m_avr_bitrate)
        m_audioFileDuration = 8 * ((float)m_audioDataSize / m_avr_bitrate); // #289
    else
        return 0;
    return m_audioFileDuration;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getAudioCurrentTime()
{ // return current time in seconds
    return (uint32_t)m_audioCurrentTime;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setAudioPlayPosition(uint16_t sec)
{
    // Jump to an absolute position in time within an audio file
    // e.g. setAudioPlayPosition(300) sets the pointer at pos 5 min
    if (sec > getAudioFileDuration())
        sec = getAudioFileDuration();
    uint32_t filepos = m_audioDataStart + (m_avr_bitrate * sec / 8);
    return setFilePos(filepos);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setVolumeSteps(uint8_t steps)
{
    m_vol_steps = steps;
    if (steps < 1)
        m_vol_steps = 64; /* avoid div-by-zero :-) */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Audio::maxVolume() { return m_vol_steps; };
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::getTotalPlayingTime()
{
    // Is set to zero by a connectToXXX() and starts as soon as the first audio data is available,
    // the time counting is not interrupted by a 'pause / resume' and is not reset by a fileloop
    return millis() - m_PlayingStartTime;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setTimeOffset(int sec)
{
    // fast forward or rewind the current position in seconds
    // audiosource must be a mp3, aac or wav file

    if (!audiofile || !m_avr_bitrate)
        return false;

    uint32_t oneSec = m_avr_bitrate / 8;                 // bytes decoded in one sec
    int32_t offset = oneSec * sec;                       // bytes to be wind/rewind
    uint32_t startAB = m_audioDataStart;                 // audioblock begin
    uint32_t endAB = m_audioDataStart + m_audioDataSize; // audioblock end

    int32_t pos = getFilePos() - inBufferFilled();
    pos += offset;
    if (pos < (int32_t)startAB)
        pos = startAB;
    if (pos >= (int32_t)endAB)
        pos = endAB;
    setFilePos(pos);

    return true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setFilePos(uint32_t pos)
{
    if (!audiofile)
        return false;
    if (pos < m_audioDataStart)
        pos = m_audioDataStart; // issue #96
    if (pos > m_file_size)
        pos = m_file_size;
    m_resumeFilePos = pos;
    memset(m_outBuff, 0, m_outbuffSize);
    m_validSamples = 0;
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setSampleRate(uint32_t sampRate)
{
    if (!sampRate)
        sampRate = 16000; // fuse, if there is no value -> set default #209
#if ESP_IDF_VERSION_MAJOR == 5
    m_i2s_std_cfg.clk_cfg.sample_rate_hz = sampRate;
    i2s_channel_reconfig_std_clock(m_i2s_tx_handle, &m_i2s_std_cfg.clk_cfg);
#else
    i2s_set_sample_rates((i2s_port_t)m_i2s_num, sampRate);
#endif
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::setBitrate(int br)
{
    m_bitRate = br;
    if (br)
        return true;
    return false;
}

uint32_t Audio::getBitRate(bool avg)
{
    if (avg)
        return m_avr_bitrate;
    return m_bitRate;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::playSample(int16_t sample[2])
{
    uint32_t s32 = Gain(sample); // sample2volume;

    if (m_f_internalDAC)
    {
        s32 += 0x80008000;
    }
    m_i2s_bytesWritten = 0;
#if (ESP_IDF_VERSION_MAJOR == 5)
    esp_err_t err = i2s_channel_write(m_i2s_tx_handle, (const char *)&s32, sizeof(uint32_t), &m_i2s_bytesWritten, 0);
#else
    esp_err_t err = i2s_write((i2s_port_t)m_i2s_num, (const char *)&s32, sizeof(uint32_t), &m_i2s_bytesWritten, 0); // no wait
#endif
    if (err != ESP_OK)
    {
        if (err != 263)
        {
            log_e("ESP32 Errorcode: %i", err);
        }
        return false;
    }
    if (m_i2s_bytesWritten < 4)
    { // no more space in dma buffer  --> break and try it later
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::forceMono(bool m)
{                      // #100 mono option
    m_f_forceMono = m; // false stereo, true mono
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setBalance(int8_t bal)
{ // bal -16...16
    if (bal < -16)
        bal = -16;
    if (bal > 16)
        bal = 16;
    m_balance = bal;

    computeLimit();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::setVolume(uint8_t vol, uint8_t curve)
{ // curve 0: default, curve 1: flat at the beginning

    uint16_t v = ESP_ARDUINO_VERSION_MAJOR * 100 + ESP_ARDUINO_VERSION_MINOR * 10 + ESP_ARDUINO_VERSION_PATCH;
    if (v < 207)
        log_e("Do not use this ancient Adruino version V%d.%d.%d", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);

    if (vol > m_vol_steps)
        m_vol = m_vol_steps;
    else
        m_vol = vol;

    if (curve > 1)
        m_curve = 1;
    else
        m_curve = curve;

    computeLimit();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Audio::getVolume() { return m_vol; }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t Audio::getI2sPort() { return m_i2s_num; }
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Audio::computeLimit()
{                               // is calculated when the volume or balance changes
    double l = 1, r = 1, v = 1; // assume 100%

    /* balance is left -16...+16 right */
    if (m_balance < 0)
    {
        r -= (double)abs(m_balance) / 16;
    }
    else if (m_balance > 0)
    {
        l -= (double)abs(m_balance) / 16;
    }

    switch (m_curve)
    {
    case 0:
        v = (double)pow(m_vol, 2) / pow(m_vol_steps, 2); // square (default)
        break;
    case 1: // logarithmic
        double log1 = log(1);
        if (m_vol > 0)
        {
            v = m_vol * ((std::exp(log1 + (m_vol - 1) * (std::log(m_vol_steps) - log1) / (m_vol_steps - 1))) / m_vol_steps) / m_vol_steps;
        }
        else
        {
            v = 0;
        }
        break;
    }

    m_limit_left = l * v;
    m_limit_right = r * v;

    // log_i("m_limit_left %f,  m_limit_right %f ",m_limit_left, m_limit_right);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int32_t Audio::Gain(int16_t s[2])
{
    int32_t v[2];

    /* important: these multiplications must all be signed ints, or the result will be invalid */
    v[LEFTCHANNEL] = s[LEFTCHANNEL] * m_limit_left;
    v[RIGHTCHANNEL] = s[RIGHTCHANNEL] * m_limit_right;

    return (v[LEFTCHANNEL] << 16) | (v[RIGHTCHANNEL] & 0xffff);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::inBufferFilled()
{
    // current audio input buffer fillsize in bytes
    return InBuff.bufferFilled();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::inBufferFree()
{
    // current audio input buffer free space in bytes
    return InBuff.freeSpace();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t Audio::inBufferSize()
{
    // current audio input buffer size in bytes
    return InBuff.getBufsize();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Audio::readID3V1Tag()
{
    // this is an V1.x id3tag after an audio block, ID3 v1 tags are ASCII
    // Version 1.x is a fixed size at the end of the file (128 bytes) after a <TAG> keyword.

    if (InBuff.bufferFilled() == 128 && startsWith((const char *)InBuff.getReadPtr(), "TAG"))
    { // maybe a V1.x TAG
        return true;
    }
    if (InBuff.bufferFilled() == 227 && startsWith((const char *)InBuff.getReadPtr(), "TAG+"))
    { // ID3V1EnhancedTAG
        return true;
    }
    return false;
}

uint32_t Audio::mp3_correctResumeFilePos(uint32_t resumeFilePos)
{
    // The starting point is the next MP3 syncword
    uint8_t p1, p2;
    boolean found = false;
    uint32_t pos = resumeFilePos;
    audiofile.seek(pos);

    p1 = audiofile.read();
    p2 = audiofile.read();
    pos += 2;
    while (!found || pos == m_file_size)
    {
        if (p1 == 0xFF && (p2 & 0xF0) == 0xF0)
        {
            found = true;
            break;
        }
        p1 = p2;
        p2 = audiofile.read();
        pos++;
    }
    MP3Decoder_ClearBuffer();
    if (found)
        return (pos - 2);
    return m_audioDataStart;
}

int Audio::read_ID3_Header(uint8_t *data, size_t len)
{
    static size_t id3Size;
    static size_t remainingHeaderBytes;
    static size_t universal_tmp = 0;
    static uint8_t ID3version;
    static int ehsz = 0;
    static char tag[5];
    static char frameid[5];
    static size_t framesize = 0;
    static bool compressed = false;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 0)
    { /* read ID3 tag and ID3 header size */
        ID3version = 0;
        m_contentlength = getFileSize();
        m_controlCounter++;
        remainingHeaderBytes = 0;
        ehsz = 0;
        if (specialIndexOf(data, "ID3", 4) != 0)
        {              // ID3 not found
            return -1; // error, no ID3 signature found
        }
        ID3version = *(data + 3);
        switch (ID3version)
        {
        case 2:
            m_f_unsync = (*(data + 5) & 0x80);
            m_f_exthdr = false;
            break;
        case 3:
        case 4:
            m_f_unsync = (*(data + 5) & 0x80); // bit7
            m_f_exthdr = (*(data + 5) & 0x40); // bit6 extended header
            break;
        };
        id3Size = bigEndian(data + 6, 4, 7); //  ID3v2 size  4 * %0xxxxxxx (shift left seven times!!)
        id3Size += 10;

        // Every read from now may be unsync'd

        if (ID3version == 2)
        {
            m_controlCounter = 10;
        }
        remainingHeaderBytes = id3Size;
        remainingHeaderBytes -= 10;

        return 10;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 1)
    { // compute extended header size if exists
        m_controlCounter++;
        if (m_f_exthdr)
        {
            ehsz = bigEndian(data, 4);
            remainingHeaderBytes -= 4;
            ehsz -= 4;
            return 4;
        }
        else
            return 0;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 2)
    { // skip extended header if exists
        if (ehsz > len)
        {
            ehsz -= len;
            remainingHeaderBytes -= len;
            return len;
        } // Throw it away
        else
        {
            m_controlCounter++;
            remainingHeaderBytes -= ehsz;
            return ehsz;
        } // Throw it away
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 3)
    { // read a ID3 frame, get the tag
        if (remainingHeaderBytes == 0)
        {
            m_controlCounter = 99;
            return 0;
        }
        m_controlCounter++;
        frameid[0] = *(data + 0);
        frameid[1] = *(data + 1);
        frameid[2] = *(data + 2);
        frameid[3] = *(data + 3);
        frameid[4] = 0;
        for (uint8_t i = 0; i < 4; ++i)
            tag[i] = frameid[i]; // tag = frameid

        remainingHeaderBytes -= 4;
        if (frameid[0] == 0 && frameid[1] == 0 && frameid[2] == 0 && frameid[3] == 0)
        {
            // We're in padding
            m_controlCounter = 98; // all ID3 metadata processed
        }
        return 4;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 4)
    { // get the frame size
        m_controlCounter = 6;

        if (ID3version == 4)
            framesize = bigEndian(data, 4, 7); // << 7
        else
            framesize = bigEndian(data, 4); // << 8

        remainingHeaderBytes -= 4;
        uint8_t flag = *(data + 4); // skip 1st flag
        (void)flag;
        remainingHeaderBytes--;
        compressed = (*(data + 5)) & 0x80; // Frame is compressed using [#ZLIB zlib] with 4 bytes for 'decompressed
                                           // size' appended to the frame header.
        remainingHeaderBytes--;
        uint32_t decompsize = 0;
        if (compressed)
        {
            decompsize = bigEndian(data + 6, 4);
            remainingHeaderBytes -= 4;
            (void)decompsize;
            return 6 + 4;
        }
        return 6;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 5)
    { // If the frame is larger than 512 bytes, skip the rest
        if (framesize > len)
        {
            framesize -= len;
            remainingHeaderBytes -= len;
            return len;
        }
        else
        {
            m_controlCounter = 3; // check next frame
            remainingHeaderBytes -= framesize;
            return framesize;
        }
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 6)
    {                         // Read the value
        m_controlCounter = 5; // only read 256 bytes
        char ch = *(data + 0);
        // $00 – ISO-8859-1 (LATIN-1, Identical to ASCII for values smaller than 0x80).
        // $01 – UCS-2 encoded Unicode with BOM, in ID3v2.2 and ID3v2.3.
        // $02 – UTF-16BE encoded Unicode without BOM, in ID3v2.4.
        // $03 – UTF-8 encoded Unicode, in ID3v2.4.
        bool isUnicode = (ch == 1) ? true : false;

        if (startsWith(tag, "APIC"))
        { // a image embedded in file, passing it to external function
            isUnicode = false;
            return 0;
        }

        if (startsWith(tag, "SYLT") || startsWith(tag, "TXXX") || startsWith(tag, "USLT"))
        {
            return 0;
        }

        size_t fs = framesize;
        if (fs > 1024)
            fs = 1024;
        for (int i = 0; i < fs; ++i)
        {
            m_ibuff[i] = *(data + i);
        }
        framesize -= fs;
        remainingHeaderBytes -= fs;
        m_ibuff[fs] = 0;

        if (isUnicode && fs > 1)
        {
            unicode2utf8(m_ibuff, fs); // convert unicode to utf-8 U+0020...U+07FF
        }

        if (!isUnicode)
        {
            uint16_t j = 0, k = 0;
            while (j < fs)
            {
                if (m_ibuff[j] > 0x1F)
                {
                    m_ibuff[k] = m_ibuff[j]; // remove non printables
                    ++k;
                }
                ++j;
            }
            m_ibuff[k] = '\0'; // new termination
            latinToUTF8(m_ibuff, k - 1);
        }
        return fs;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // --- section V2.2 only , greater Vers above ----
    // see https://mutagen-specs.readthedocs.io/en/latest/id3/id3v2.2.html
    if (m_controlCounter == 10)
    { // frames in V2.2, 3bytes identifier, 3bytes size descriptor

        if (universal_tmp > 0)
        {
            if (universal_tmp > 256)
            {
                universal_tmp -= 256;
                return 256;
            }
            else
            {
                uint8_t t = universal_tmp;
                universal_tmp = 0;
                return t;
            }
        }

        frameid[0] = *(data + 0);
        frameid[1] = *(data + 1);
        frameid[2] = *(data + 2);
        frameid[3] = 0;
        for (uint8_t i = 0; i < 4; ++i)
            tag[i] = frameid[i]; // tag = frameid
        remainingHeaderBytes -= 3;
        size_t dataLen = bigEndian(data + 3, 3);
        universal_tmp = dataLen;
        remainingHeaderBytes -= 3;

        if (dataLen > 249)
        {
            dataLen = 249;
        }

        m_chbuf[0] = 0;

        remainingHeaderBytes -= universal_tmp;
        universal_tmp -= dataLen;

        if (dataLen == 0)
            m_controlCounter = 98;
        if (remainingHeaderBytes == 0)
            m_controlCounter = 98;

        return 3 + 3 + dataLen;
    }
    // -- end section V2.2 -----------

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 98)
    { // skip all ID3 metadata (mostly spaces)
        if (remainingHeaderBytes > len)
        {
            remainingHeaderBytes -= len;
            return len;
        } // Throw it away
        else
        {
            m_controlCounter = 99;
            return remainingHeaderBytes;
        } // Throw it away
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (m_controlCounter == 99)
    { //  exist another ID3tag?
        m_audioDataStart += id3Size;
        if ((*(data + 0) == 'I') && (*(data + 1) == 'D') && (*(data + 2) == '3'))
        {
            m_controlCounter = 0;
            return 0;
        }
        else
        {
            m_controlCounter = 100; // ok
            m_audioDataSize = m_contentlength - m_audioDataStart;
            return 0;
        }
    }
    return 0;
}
