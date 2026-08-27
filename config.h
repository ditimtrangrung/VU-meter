#ifndef CONFIG_H_
#define CONFIG_H_

/* ===================== CẤU HÌNH CHẾ ĐỘ ===================== */
#define STEREO              1

/* ===================== THÔNG SỐ VU (Độ nhạy tăng) ===================== */
#define NUMBER_OF_LED       8
#define AVERAGE_SAMPLES     16          /* Giảm từ 32 → 16 (phản ứng nhanh hơn) */
#define ATTACK_COEF         0.95f       /* Tăng từ 0.9 → 0.95 (bắt tín hiệu nhanh hơn) */
#define RELEASE_COEF        0.10f       /* Tăng từ 0.05 → 0.10 (rơi nhanh hơn) */
#define PEAK_HOLD_MS        100
#define PEAK_FALL_RATE      1.20f       /* Tăng từ 0.80 → 1.20 (peak rơi nhanh hơn) */
#define UPDATE_INTERVAL_MS  12

/* ===================== GAIN DIGITAL ===================== */
#define DEFAULT_GAIN_L      6.00f       /* Tăng từ 4.50 → 6.00 (nhạy hơn) */
#define DEFAULT_GAIN_R      6.00f       /* Tăng từ 4.50 → 6.00 (nhạy hơn) */
#define GAIN_MIN            0.30f
#define GAIN_MAX            8.00f       /* Tăng từ 5.00 → 8.00 (gain tối đa lớn hơn) */

/* ===================== PIN ADC ===================== */
#define ADC_CH_L            ADC_Channel_1   /* PA1 - Kênh Trái  */
#define ADC_CH_R            ADC_Channel_0   /* PA0 - Kênh Phải  */

/* ===================== PIN 74HC595 ===================== */
#define HC595_SER_L_PORT    GPIOC
#define HC595_SER_L_PIN     GPIO_Pin_3

#define HC595_SER_R_PORT    GPIOC
#define HC595_SER_R_PIN     GPIO_Pin_4

#define HC595_SCK_PORT      GPIOC
#define HC595_SCK_PIN       GPIO_Pin_5

#define HC595_RCK_PORT      GPIOC
#define HC595_RCK_PIN       GPIO_Pin_6

/* ===================== JUMPER CHỌN MODE =====================
 * JP1 = PD5 (pin 2 CPU), JP2 = PD6 (pin 3 CPU)
 * Cấu hình Input Pull-Up nội bộ:
 *   Jumper HỞ  = chân ở mức HIGH (pull-up kéo lên)
 *   Jumper GND = chân ở mức LOW  (jumper nối xuống GND)
 *
 *  JP1(PD5)  JP2(PD6)  Mode  Chức năng
 *  Hở        Hở     →  0   SMOOTH_NOPEAK : Bar rơi chậm, không peak — MẶC ĐỊNH
 *  Hở        GND    →  1   DOT_WATERFALL : Dot + Waterfall peak rơi từng LED
 *  GND       Hở     →  2   BAR_WATERFALL : Bar + Waterfall peak rơi từng LED
 *  GND       GND    →  3   SMOOTH_PEAK   : Bar + Peak hold, rơi chậm mượt
 * ============================================================= */
#define JP1_PORT            GPIOD
#define JP1_PIN             GPIO_Pin_5   /* PD5 - pin 2 CPU */

#define JP2_PORT            GPIOD
#define JP2_PIN             GPIO_Pin_6   /* PD6 - pin 3 CPU */

/* ===================== FLASH STORAGE ===================== */
#define FLASH_STORAGE_ADDR  0x08003C00
#define FLASH_MAGIC         0xA5A5C003

#endif /* CONFIG_H_ */
