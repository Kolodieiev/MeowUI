/*
 * Audio.h
 *
 *  Created on: Oct 28,2018
 *
 *  Version 3.0.8p
 *  Updated on: Feb 20.2024
 *      Author: Wolle (schreibfaul1)
 */

/* Примітка. Код відредаговано by Пройдисвіт. Видалено підтримку будь-яких форматів, окрім mp3. */

#pragma once
#pragma GCC optimize("Ofast")
#include <Arduino.h>
#include <FS.h>
#include <atomic>

#if ESP_IDF_VERSION_MAJOR == 5
#include <driver/i2s_std.h>
#else
#include <driver/i2s.h>
#endif

#ifndef I2S_GPIO_UNUSED
#define I2S_GPIO_UNUSED -1 // = I2S_PIN_NO_CHANGE in IDF < 5
#endif
using namespace std;

//----------------------------------------------------------------------------------------------------------------------

class AudioBuffer
{

public:
    AudioBuffer(size_t maxBlockSize = 0); // constructor
    ~AudioBuffer();                       // frees the buffer
    size_t init();                        // set default values
    bool isInitialized() { return m_f_init; };
    void setBufsize(int ram, int psram);
    int32_t getBufsize();
    void changeMaxBlockSize(uint16_t mbs); // is default 1600 for mp3 and aac, set 16384 for FLAC
    uint16_t getMaxBlockSize();            // returns maxBlockSize
    size_t freeSpace();                    // number of free bytes to overwrite
    size_t writeSpace();                   // space fom writepointer to bufferend
    size_t bufferFilled();                 // returns the number of filled bytes
    size_t getMaxAvailableBytes();         // max readable bytes in one block
    void bytesWritten(size_t bw);          // update writepointer
    void bytesWasRead(size_t br);          // update readpointer
    uint8_t *getWritePtr();                // returns the current writepointer
    uint8_t *getReadPtr();                 // returns the current readpointer
    uint32_t getWritePos();                // write position relative to the beginning
    uint32_t getReadPos();                 // read position relative to the beginning
    void resetBuffer();                    // restore defaults
    bool havePSRAM() { return m_f_psram; };

protected:
    size_t m_buffSizePSRAM = UINT16_MAX * 10; // most webstreams limit the advance to 100...300Kbytes
    size_t m_buffSizeRAM = 1600 * 10;
    size_t m_buffSize = 0;
    size_t m_freeSpace = 0;
    size_t m_writeSpace = 0;
    size_t m_dataLength = 0;
    size_t m_resBuffSizeRAM = 2048;       // reserved buffspace, >= one wav  frame
    size_t m_resBuffSizePSRAM = 4096 * 4; // reserved buffspace, >= one flac frame
    size_t m_maxBlockSize = 1600;
    uint8_t *m_buffer = NULL;
    uint8_t *m_writePtr = NULL;
    uint8_t *m_readPtr = NULL;
    uint8_t *m_endPtr = NULL;
    bool m_f_start = true;
    bool m_f_init = false;
    bool m_f_psram = false; // PSRAM is available (and used...)
};
//----------------------------------------------------------------------------------------------------------------------

class Audio : private AudioBuffer
{

    AudioBuffer InBuff; // instance of input buffer

public:
    Audio(bool internalDAC = false, uint8_t channelEnabled = 3, uint8_t i2sPort = I2S_NUM_0); // #99
    ~Audio();
    void setBufsize(int rambuf_sz, int psrambuf_sz);

    bool connecttoFS(fs::FS &fs, const char *path, int32_t resumeFilePos = -1);
    bool setFileLoop(bool input); // TEST loop

    int read_ID3_Header(uint8_t *data, size_t len);

    bool setAudioPlayPosition(uint16_t sec);
    bool setFilePos(uint32_t pos);
    bool setTimeOffset(int sec);
    bool setPinout(uint8_t BCLK, uint8_t LRC, uint8_t DOUT, int8_t MCLK = I2S_GPIO_UNUSED);
    bool pauseResume();
    bool isRunning() { return m_f_running; }
    void loop();
    uint32_t stopSong();
    void forceMono(bool m);
    void setBalance(int8_t bal = 0);
    void setVolumeSteps(uint8_t steps);
    void setVolume(uint8_t vol, uint8_t curve = 0);
    uint8_t getVolume();
    uint8_t maxVolume();
    uint8_t getI2sPort();

    uint32_t getAudioDataStartPos();
    uint32_t getFileSize();
    uint32_t getFilePos();
    uint32_t getBitRate(bool avg = false);
    uint32_t getAudioFileDuration();
    uint32_t getAudioCurrentTime();
    uint32_t getTotalPlayingTime();

    uint32_t inBufferFilled(); // returns the number of stored bytes in the inputbuffer
    uint32_t inBufferFree();   // returns the number of free bytes in the inputbuffer
    uint32_t inBufferSize();   // returns the size of the inputbuffer in bytes
    void unicode2utf8(char *buff, uint32_t len);

private:
#ifndef ESP_ARDUINO_VERSION_VAL
#define ESP_ARDUINO_VERSION_MAJOR 0
#define ESP_ARDUINO_VERSION_MINOR 0
#define ESP_ARDUINO_VERSION_PATCH 0
#endif

    void UTF8toASCII(char *str);
    bool latinToUTF8(char *buff, size_t bufflen);
    void setDefaults(); // free buffers and set defaults
    void initInBuff();
    void processLocalFile();
    void playAudioData();
    int findNextSync(uint8_t *data, size_t len);
    int sendBytes(uint8_t *data, size_t len);
    void setDecoderItems();
    void compute_audioCurrentTime(int bd);
    size_t readAudioHeader(uint32_t bytes);
    bool setSampleRate(uint32_t hz);

    bool setBitrate(int br);
    void playChunk();
    bool playSample(int16_t sample[2]);
    void computeLimit();
    int32_t Gain(int16_t s[2]);

    bool initializeDecoder();
    esp_err_t I2Sstart(uint8_t i2s_num);
    esp_err_t I2Sstop(uint8_t i2s_num);

    bool readID3V1Tag();

    uint32_t mp3_correctResumeFilePos(uint32_t resumeFilePos);

    //++++ implement several function with respect to the index of string ++++
    void strlower(char *str)
    {
        unsigned char *p = (unsigned char *)str;
        while (*p)
        {
            *p = tolower((unsigned char)*p);
            ++p;
        }
    }

    void trim(char *s)
    {
        // fb   trim in place
        char *pe;
        char *p = s;
        while (isspace(*p))
            ++p; // left
        pe = p;  // right
        while (*pe != '\0')
            ++pe;
        do
        {
            pe--;
        } while ((pe > p) && isspace(*pe));
        if (p == s)
        {
            *++pe = '\0';
        }
        else
        { // move
            while (p <= pe)
                *s++ = *p++;
            *s = '\0';
        }
    }

    bool startsWith(const char *base, const char *str)
    {
        // fb
        char c;
        while ((c = *str++) != '\0')
            if (c != *base++)
                return false;
        return true;
    }

    bool endsWith(const char *base, const char *searchString)
    {
        int32_t slen = strlen(searchString);
        if (slen == 0)
            return false;
        const char *p = base + strlen(base);
        //  while(p > base && isspace(*p)) p--;  // rtrim
        p -= slen;
        if (p < base)
            return false;
        return (strncmp(p, searchString, slen) == 0);
    }

    int indexOf(const char *base, const char *str, int startIndex = 0)
    {
        // fb
        const char *p = base;
        for (; startIndex > 0; startIndex--)
            if (*p++ == '\0')
                return -1;
        char *pos = strstr(p, str);
        if (pos == nullptr)
            return -1;
        return pos - base;
    }

    int indexOf(const char *base, char ch, int startIndex = 0)
    {
        // fb
        const char *p = base;
        for (; startIndex > 0; startIndex--)
            if (*p++ == '\0')
                return -1;
        char *pos = strchr(p, ch);
        if (pos == nullptr)
            return -1;
        return pos - base;
    }

    int lastIndexOf(const char *haystack, const char *needle)
    {
        // fb
        int nlen = strlen(needle);
        if (nlen == 0)
            return -1;
        const char *p = haystack - nlen + strlen(haystack);
        while (p >= haystack)
        {
            int i = 0;
            while (needle[i] == p[i])
                if (++i == nlen)
                    return p - haystack;
            p--;
        }
        return -1;
    }

    int lastIndexOf(const char *haystack, const char needle)
    {
        // fb
        const char *p = strrchr(haystack, needle);
        return (p ? p - haystack : -1);
    }

    int specialIndexOf(uint8_t *base, const char *str, int baselen, bool exact = false)
    {
        int result = 0; // seek for str in buffer or in header up to baselen, not nullterninated
        if (strlen(str) > baselen)
            return -1; // if exact == true seekstr in buffer must have "\0" at the end
        for (int i = 0; i < baselen - strlen(str); ++i)
        {
            result = i;
            for (int j = 0; j < strlen(str) + exact; ++j)
            {
                if (*(base + i + j) != *(str + j))
                {
                    result = -1;
                    break;
                }
            }
            if (result >= 0)
                break;
        }
        return result;
    }

    // some other functions
    size_t bigEndian(uint8_t *base, uint8_t numBytes, uint8_t shiftLeft = 8)
    {
        uint64_t result = 0;
        if (numBytes < 1 || numBytes > 8)
            return 0;
        for (int i = 0; i < numBytes; ++i)
        {
            result += *(base + i) << (numBytes - i - 1) * shiftLeft;
        }
        if (result > SIZE_MAX)
        {
            log_e("range overflow");
            result = 0;
        } // overflow
        return (size_t)result;
    }

private:
    typedef enum
    {
        LEFTCHANNEL = 0,
        RIGHTCHANNEL = 1
    } SampleIndex;

    File audiofile; // @suppress("Abstract class cannot be instantiated")
    SemaphoreHandle_t mutex_audio;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#if ESP_IDF_VERSION_MAJOR == 5
    i2s_chan_handle_t m_i2s_tx_handle = {};
    i2s_chan_config_t m_i2s_chan_cfg = {}; // stores I2S channel values
    i2s_std_config_t m_i2s_std_cfg = {};   // stores I2S driver values
#else
    i2s_config_t m_i2s_config = {};
    i2s_pin_config_t m_pin_config = {};
#endif
#pragma GCC diagnostic pop

    const size_t m_frameSizeMP3 = 1600;
    const size_t m_outbuffSize = 4096 * 2;

    static const uint8_t m_tsPacketSize = 188;
    static const uint8_t m_tsHeaderSize = 4;

    char *m_ibuff = nullptr; // used in audio_info()
    char *m_chbuf = NULL;
    uint16_t m_chbufSize = 0;   // will set in constructor (depending on PSRAM)
    uint16_t m_ibuffSize = 0;   // will set in constructor (depending on PSRAM)
    uint32_t m_bitRate = 0;     // current bitrate given fom decoder
    uint32_t m_avr_bitrate = 0; // average bitrate, median computed by VBR
    int m_controlCounter = 0;   // Status within readID3data() and readWaveHeader()
    int8_t m_balance = 0;       // -16 (mute left) ... +16 (mute right)
    uint16_t m_vol = 21;        // volume
    uint8_t m_vol_steps = 21;   // default
    double m_limit_left = 0;    // limiter 0 ... 1, left channel
    double m_limit_right = 0;   // limiter 0 ... 1, right channel
    uint8_t m_curve = 0;        // volume characteristic
    uint8_t m_channels = 2;
    uint8_t m_i2s_num = I2S_NUM_0; // I2S_NUM_0 or I2S_NUM_1

    int16_t *m_outBuff = NULL;                 // Interleaved L/R
    std::atomic<int16_t> m_validSamples = {0}; // #144
    std::atomic<int16_t> m_curSample{0};
    int16_t m_decodeError = 0; // Stores the return value of the decoder
    uint16_t m_timeout_ms = 250;
    uint16_t m_timeout_ms_ssl = 2700;

    uint32_t m_metaint = 0;          // Number of databytes between metadata
    uint32_t m_chunkcount = 0;       // Counter for chunked transfer
    uint32_t m_t0 = 0;               // store millis(), is needed for a small delay
                                     // uint32_t        m_byteCounter = 0;              // count received data
    uint32_t m_contentlength = 0;    // Stores the length if the stream comes from fileserver
    uint32_t m_bytesNotDecoded = 0;  // pictures or something else that comes with the stream
    uint32_t m_PlayingStartTime = 0; // Stores the milliseconds after the start of the audio
    int32_t m_resumeFilePos = -1;    // the return value from stopSong() can be entered here, (-1) is idle
    uint32_t m_stsz_numEntries = 0;  // num of entries inside stsz atom (uint32_t)
    uint32_t m_stsz_position = 0;    // pos of stsz atom within file
    bool m_f_metadata = false;       // assume stream without metadata
    bool m_f_unsync = false;         // set within ID3 tag but not used
    bool m_f_exthdr = false;         // ID3 extended header
    bool m_f_ssl = false;
    bool m_f_running = false;
    bool m_f_firstCall = false;     // InitSequence for processWebstream and processLokalFile
    bool m_f_playing = false;       // valid mp3 stream recognized
    bool m_f_tts = false;           // text to speech
    bool m_f_loop = false;          // Set if audio file should loop
    bool m_f_forceMono = false;     // if true stereo -> mono
    bool m_f_internalDAC = false;   // false: output vis I2S, true output via internal DAC
    bool m_f_Log = false;           // set in platformio.ini  -DAUDIO_LOG and -DCORE_DEBUG_LEVEL=3 or 4
    bool m_f_psramFound = false;    // set in constructor, result of psramInit()
    uint8_t m_f_channelEnabled = 3; // internal DAC, both channels
    uint32_t m_audioFileDuration = 0;
    float m_audioCurrentTime = 0;
    uint32_t m_audioDataStart = 0; // in bytes
    size_t m_audioDataSize = 0;    //
    size_t m_i2s_bytesWritten = 0; // set in i2s_write() but not used
    size_t m_file_size = 0;        // size of the file
};

//----------------------------------------------------------------------------------------------------------------------
