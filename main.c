#include "debug.h"
#include "config.h"

#define MY_FABS(x)  ((x) < 0.0f ? -(x) : (x))

#define SET_BIT(port, pin)    ((port)->BSHR = (1U << (pin)))
#define CLR_BIT(port, pin)    ((port)->BCR  = (1U << (pin)))
#define SET_BITS(port, mask)  ((port)->BSHR = (mask))
#define CLR_BITS(port, mask)  ((port)->BCR  = (mask))

typedef struct {
    float    avg;
    float    level;
    float    peak;
    uint16_t hold_cnt;
} channel_t;

channel_t chL = {0.0f, 0.0f, 0.0f, 0};
channel_t chR = {0.0f, 0.0f, 0.0f, 0};

float gain_L = DEFAULT_GAIN_L;
float gain_R = DEFAULT_GAIN_R;
float adc_offset_L = 512.0f;
float adc_offset_R = 512.0f;

/* ===================== Đọc Mode từ JP1, JP2 =====================
 * JP1=PD5, JP2=PD6, cấu hình Input Pull-Up.
 * Hở=HIGH, GND=LOW.
 *
 * Trả về:
 *   0 = SMOOTH_NOPEAK  (JP1 hở, JP2 hở)  — mặc định
 *   1 = DOT_WATERFALL  (JP1 hở, JP2 GND)
 *   2 = BAR_WATERFALL  (JP1 GND, JP2 hở)
 *   3 = SMOOTH_PEAK    (JP1 GND, JP2 GND)
 * ================================================================= */
static uint8_t Read_Mode(void)
{
    uint8_t jp1 = (GPIO_ReadInputDataBit(JP1_PORT, JP1_PIN) == Bit_RESET); /* GND=1, hở=0 */
    uint8_t jp2 = (GPIO_ReadInputDataBit(JP2_PORT, JP2_PIN) == Bit_RESET); /* GND=1, hở=0 */
    return (uint8_t)((jp1 << 1) | jp2);
    /* jp1=0,jp2=0 → 0  SMOOTH_NOPEAK
     * jp1=0,jp2=1 → 1  DOT_WATERFALL
     * jp1=1,jp2=0 → 2  BAR_WATERFALL
     * jp1=1,jp2=1 → 3  SMOOTH_PEAK   */
}

/* ===================== Đảo bit =====================
 * Cả 2 IC 74HC595 trên PCB đi dây LED từ QH→QA
 * nên cần đảo bit trước khi gửi.
 * ===================================================== */
static uint8_t ReverseBits(uint8_t b)
{
    b = (uint8_t)((b & 0xF0) >> 4 | (b & 0x0F) << 4);
    b = (uint8_t)((b & 0xCC) >> 2 | (b & 0x33) << 2);
    b = (uint8_t)((b & 0xAA) >> 1 | (b & 0x55) << 1);
    return b;
}

/* ===================== 74HC595 ===================== */
void HC595_Send8x2(uint8_t dataL, uint8_t dataR)
{
    dataL = ReverseBits(dataL);
    dataR = ReverseBits(dataR);

    CLR_BIT(GPIOC, 6);
    CLR_BIT(GPIOC, 5);

    for (volatile int d = 0; d < 50; d++) __NOP();

    for (int i = 0; i < 8; i++)
    {
        if (dataL & (1U << i)) SET_BIT(GPIOC, 3);
        else                   CLR_BIT(GPIOC, 3);

        if (dataR & (1U << i)) SET_BIT(GPIOC, 4);
        else                   CLR_BIT(GPIOC, 4);

        for (volatile int d = 0; d < 50; d++) __NOP();

        SET_BIT(GPIOC, 5);
        for (volatile int d = 0; d < 80; d++) __NOP();
        CLR_BIT(GPIOC, 5);
        for (volatile int d = 0; d < 50; d++) __NOP();
    }

    CLR_BITS(GPIOC, (1U<<3) | (1U<<4));
    for (volatile int d = 0; d < 50; d++) __NOP();

    SET_BIT(GPIOC, 6);
    for (volatile int d = 0; d < 80; d++) __NOP();
    CLR_BIT(GPIOC, 6);
}

/* ===================== GPIO ===================== */
void GPIO_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC |
                           RCC_APB2Periph_GPIOD, ENABLE);

    /* HC595: PC3,PC4,PC5,PC6 — Output Push-Pull 50MHz */
    GPIOC->CFGLR &= ~((0xFU<<(3*4)) | (0xFU<<(4*4)) | (0xFU<<(5*4)) | (0xFU<<(6*4)));
    GPIOC->CFGLR |=  ((0x3U<<(3*4)) | (0x3U<<(4*4)) | (0x3U<<(5*4)) | (0x3U<<(6*4)));
    CLR_BITS(GPIOC, (1U<<3)|(1U<<4)|(1U<<5)|(1U<<6));

    /* ADC: PA0, PA1 — Analog Input */
    GPIO_InitTypeDef g = {0};
    g.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
    g.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &g);

    /* JP1=PD5, JP2=PD6 — Input Pull-Up
     * Hở = HIGH (pull-up nội), GND = LOW (jumper cắm) */
    g.GPIO_Pin  = JP1_PIN | JP2_PIN;
    g.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(JP1_PORT, &g);
}

/* ===================== ADC ===================== */
void ADC_Config(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    ADC_InitTypeDef a = {0};
    a.ADC_Mode               = ADC_Mode_Independent;
    a.ADC_ScanConvMode       = DISABLE;
    a.ADC_ContinuousConvMode = DISABLE;
    a.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    a.ADC_DataAlign          = ADC_DataAlign_Right;
    a.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &a);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC_Read(uint8_t channel)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

/* ===================== ADC Offset Calibration ===================== */
static void Calibrate_ADC_Offset(void)
{
    uint32_t sumL = 0, sumR = 0;
    for (int i = 0; i < 100; i++) {
        sumL += ADC_Read(ADC_CH_L);
        sumR += ADC_Read(ADC_CH_R);
        Delay_Ms(1);
    }
    adc_offset_L = (float)sumL / 100.0f;
    adc_offset_R = (float)sumR / 100.0f;
}

/* ===================== Log2 Level ===================== */
static float Log2Level(float valued)
{
    if (valued < 1.0f) return 0.0f;
    uint16_t v = (uint16_t)valued;
    if (v > 1023) v = 1023;
    uint16_t tmp = v;
    uint8_t  msb = 0;
    while (tmp >>= 1) msb++;
    uint16_t lower = 1U << msb;
    float frac = (float)(v - lower) / (float)lower;
    float log_val = (float)msb + frac;
    float lvl = (log_val / 10.0f) * NUMBER_OF_LED;
    if (lvl > NUMBER_OF_LED) lvl = NUMBER_OF_LED;
    return lvl;
}

/* ===================== Xử lý kênh ===================== */
void ProcessChannel(channel_t *ch, uint16_t raw, float gain, float offset,
                    uint8_t peak_hold_enable, uint8_t log_scale)
{
    float valued = MY_FABS((float)raw - offset) * 2.0f * gain;
    if (valued > 1023.0f) valued = 1023.0f;

    ch->avg = (ch->avg * (AVERAGE_SAMPLES - 1) + valued) / AVERAGE_SAMPLES;

    float target_lvl;
    if (log_scale)
        target_lvl = Log2Level(ch->avg);
    else
        target_lvl = (ch->avg / 1023.0f) * NUMBER_OF_LED;

    if (target_lvl > NUMBER_OF_LED) target_lvl = NUMBER_OF_LED;

    if (target_lvl > ch->level)
        ch->level += (target_lvl - ch->level) * ATTACK_COEF;
    else
        ch->level += (target_lvl - ch->level) * RELEASE_COEF;

    if (peak_hold_enable)
    {
        if (ch->level > ch->peak)
        {
            ch->peak     = ch->level;
            ch->hold_cnt = PEAK_HOLD_MS / UPDATE_INTERVAL_MS;
        }
        else
        {
            if (ch->hold_cnt > 0)
                ch->hold_cnt--;
            else
            {
                ch->peak -= PEAK_FALL_RATE;
                if (ch->peak < ch->level) ch->peak = ch->level;
                if (ch->peak < 0.0f)     ch->peak = 0.0f;
            }
        }
    }
    else
    {
        ch->peak = ch->level;
    }
}

/* ===================== Pattern LED ===================== */
uint8_t MakePattern(float level, float peak, uint8_t mode_bar, uint8_t waterfall)
{
    uint8_t pat = 0;
    int lev = (int)(level + 0.5f);
    int pk  = (int)(peak  + 0.5f);

    if (lev > NUMBER_OF_LED) lev = NUMBER_OF_LED;
    if (pk  > NUMBER_OF_LED) pk  = NUMBER_OF_LED;
    if (pk  < lev) pk = lev;

    if (mode_bar)
        for (int i = 0; i < lev; i++) pat |= (1U << i);
    else
        if (lev > 0) pat |= (uint8_t)(1U << (lev - 1));

    if (waterfall)
        for (int i = lev; i < pk; i++) pat |= (1U << i);
    else
        if (pk > 0) pat |= (uint8_t)(1U << (pk - 1));

    return pat & 0xFF;
}

/* ===================== FLASH ===================== */
typedef struct {
    uint32_t magic;
    float    gain_L;
    float    gain_R;
    uint32_t checksum;
} __attribute__((packed)) flash_data_t;

static uint32_t Flash_CalcChecksum(flash_data_t *data)
{
    uint32_t sum = 0;
    uint32_t *ptr = (uint32_t *)data;
    for (int i = 0; i < 3; i++) sum += ptr[i];
    return sum;
}

void Flash_LoadGain(void)
{
    flash_data_t *p = (flash_data_t *)FLASH_STORAGE_ADDR;
    if ((uint32_t)p & 0x3) return;
    if (p->magic == FLASH_MAGIC && p->checksum == Flash_CalcChecksum(p)) {
        if (p->gain_L >= GAIN_MIN && p->gain_L <= GAIN_MAX) gain_L = p->gain_L;
        if (p->gain_R >= GAIN_MIN && p->gain_R <= GAIN_MAX) gain_R = p->gain_R;
    }
}

/* ===================== Startup Animation - New Wave & Pulse =====================
 * Hiệu ứng khởi động mới:
 * 1. Sóng chạy từ trái sang phải
 * 2. Pulse nhịp nhàng ở giữa
 * 3. Fade in/out kết thúc
 * ========================================================================== */
static void Startup_Effect(void)
{
    /* ===== Phase 1: Wave Left → Right (Sóng trái sang phải) ===== */
    for (int cycle = 0; cycle < 2; cycle++)
    {
        /* Sóng chạy từ trái sang phải với 3 LED */
        for (int pos = 0; pos < 8; pos++) {
            uint8_t wave = 0;
            for (int i = 0; i < 3 && (pos - i) >= 0; i++) {
                wave |= (1U << (pos - i));
            }
            HC595_Send8x2(wave, wave);
            Delay_Ms(60);
        }

        /* Sóng chạy từ phải sang trái với 3 LED */
        for (int pos = 7; pos >= 0; pos--) {
            uint8_t wave = 0;
            for (int i = 0; i < 3 && (pos + i) < 8; i++) {
                wave |= (1U << (pos + i));
            }
            HC595_Send8x2(wave, wave);
            Delay_Ms(60);
        }
    }

    Delay_Ms(100);

    /* ===== Phase 2: Pulse ở giữa (nhịp nhàng) ===== */
    for (int pulse = 0; pulse < 4; pulse++)
    {
        HC595_Send8x2(0x00, 0x00); Delay_Ms(80);
        HC595_Send8x2(0x18, 0x18); Delay_Ms(60);  /* 00011000 - 2 LED ở giữa */
        HC595_Send8x2(0x3C, 0x3C); Delay_Ms(60);  /* 00111100 - 4 LED ở giữa */
        HC595_Send8x2(0x7E, 0x7E); Delay_Ms(60);  /* 01111110 - 6 LED ở giữa */
        HC595_Send8x2(0xFF, 0xFF); Delay_Ms(100); /* 11111111 - Full */
    }

    Delay_Ms(150);

    /* ===== Phase 3: Fade in từ phải sang trái ===== */
    for (int i = 1; i <= 8; i++) {
        uint8_t pat = 0;
        for (int b = 0; b < i; b++) pat |= (1U << (7 - b));  /* Bắt đầu từ bit 7 */
        HC595_Send8x2(pat, pat);
        Delay_Ms(50);
    }

    Delay_Ms(200);

    /* ===== Phase 4: Fade out ===== */
    for (int i = 8; i >= 1; i--) {
        uint8_t pat = 0;
        for (int b = 0; b < i; b++) pat |= (1U << (7 - b));
        HC595_Send8x2(pat, pat);
        Delay_Ms(50);
    }

    HC595_Send8x2(0x00, 0x00);
    Delay_Ms(100);
}

/* ===================== Main ===================== */
int main(void)
{
    SystemCoreClockUpdate();
    Delay_Init();

    /* Reset HC595 ngay lập tức */
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
    GPIOC->CFGLR &= ~((0xFU<<(3*4))|(0xFU<<(4*4))|(0xFU<<(5*4))|(0xFU<<(6*4)));
    GPIOC->CFGLR |=  ((0x3U<<(3*4))|(0x3U<<(4*4))|(0x3U<<(5*4))|(0x3U<<(6*4)));
    CLR_BITS(GPIOC, (1U<<3)|(1U<<4)|(1U<<5)|(1U<<6));
    for (int i = 0; i < 8; i++) {
        SET_BIT(GPIOC, 5);
        for (volatile int d = 0; d < 30; d++) __NOP();
        CLR_BIT(GPIOC, 5);
        for (volatile int d = 0; d < 30; d++) __NOP();
    }
    SET_BIT(GPIOC, 6);
    for (volatile int d = 0; d < 50; d++) __NOP();
    CLR_BIT(GPIOC, 6);

    GPIO_Config();
    ADC_Config();
    HC595_Send8x2(0x00, 0x00);

    Flash_LoadGain();
    Startup_Effect();
    Calibrate_ADC_Offset();

    while (1)
    {
        /* ===== Đọc jumper mỗi vòng lặp =====
         * Cho phép thay đổi jumper khi đang chạy mà không cần reset.
         *
         * Mode 0 — SMOOTH_NOPEAK : Bar, không peak, rơi chậm (LM3915)
         * Mode 1 — DOT_WATERFALL : Dot + Waterfall, peak rơi từng LED
         * Mode 2 — BAR_WATERFALL : Bar + Waterfall, peak rơi từng LED
         * Mode 3 — SMOOTH_PEAK   : Bar + Peak hold, rơi chậm mượt
         * ===================================== */
        uint8_t mode = Read_Mode();

        uint8_t sw_mode_bar  = (mode == 0 || mode == 2 || mode == 3); /* 0,2,3=Bar / 1=Dot */
        uint8_t sw_waterfall = (mode == 1 || mode == 2);              /* 1,2=Waterfall     */
        uint8_t sw_peakhold  = (mode == 3);                           /* 3=Peak hold       */
        uint8_t sw_logscale  = 0;                                     /* Log scale tắt     */

        uint16_t rawL = ADC_Read(ADC_CH_L);
#if STEREO
        uint16_t rawR = ADC_Read(ADC_CH_R);
#else
        uint16_t rawR = rawL;
#endif

        ProcessChannel(&chL, rawL, gain_L, adc_offset_L, sw_peakhold, sw_logscale);
        ProcessChannel(&chR, rawR, gain_R, adc_offset_R, sw_peakhold, sw_logscale);

        uint8_t patL = MakePattern(chL.level, chL.peak, sw_mode_bar, sw_waterfall);
        uint8_t patR = MakePattern(chR.level, chR.peak, sw_mode_bar, sw_waterfall);

        HC595_Send8x2(patL, patR);

        Delay_Ms(UPDATE_INTERVAL_MS);
    }
}
