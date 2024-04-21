#include <Arduino.h>
#include <unity.h>

#include <TFT_eSPI.h>
TFT_eSPI _tft;

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

uint16_t getCharPos(uint16_t unicode)
{
    if (unicode > 127)
    {
        if (unicode > 1039 && unicode < 1104)
            unicode = unicode - 944;
        else
            switch (unicode)
            {
            case 1030:
                unicode = 41;
                break;
            case 1031:
                unicode = 162;
                break;
            case 1111:
                unicode = 163;
                break;
            case 1110:
                unicode = 73;
                break;
            case 1028:
                unicode = 160;
                break;
            case 1108:
                unicode = 161;
                break;
            case 1025:
                unicode = 37;
                break;
            case 1105:
                unicode = 69;
                break;
            case 1168:
                unicode = 164;
                break;
            case 1169:
                unicode = 165;
                break;
            case 171:
            case 187:
                unicode = 2;
                break;
            default:
                unicode = CHRS_NMBR - 1;
            }
    }
    else
        unicode -= 32;

    return unicode;
}

void testFont2_1()
{
    const char str1[] = "зва гри та ще ку";
    const uint16_t ASSERT_STR1_PIX_SUM{110};

    uint16_t len = strlen(str1);

    uint16_t n{0};
    uint16_t pix_sum{0};

    while (n < len)
    {
        uint16_t temp = _tft.decodeUTF8((uint8_t *)str1, &n, len - n);
        temp = getCharPos(temp);
        pix_sum += pgm_read_byte(widtbl_f2 + temp);
    }

    TEST_ASSERT_EQUAL(ASSERT_STR1_PIX_SUM, pix_sum);
}

void testFont4() {}

void setup()
{
    delay(1000);

    _tft.init();

    UNITY_BEGIN();

    RUN_TEST(testFont2_1);

    UNITY_END();
}

void loop()
{
}