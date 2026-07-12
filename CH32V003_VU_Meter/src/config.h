/**
 * @file config.h
 * @brief Cấu hình người dùng cho VU-meter CH32V003F4P6
 * @author ditimtrangrung
 * @date 2024
 *
 * Tệp cấu hình chính cho dự án VU-meter với CH32V003F4P6
 * Chỉnh sửa các tham số tại đây để thích nghi với phần cứng của bạn
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* ============================================
   CẤU HÌNH CLOCK
   ============================================ */
#define SYSTEM_CLOCK_FREQ   48000000UL  // 48 MHz

/* ============================================
   CẤU HÌNH DSP (Xử lý tín hiệu)
   ============================================ */
#define AVERAGEWIDTH        100         // Số mẫu lấy trung bình (10kHz → 100Hz)
#define PEAK_HOLD_TIME      300         // Thời gian giữ peak (ms)
#define PEAK_FALL_RATE      0.2f        // Tốc độ giảm peak

/* ============================================
   CẤU HÌNH GAIN (Khuếch đại)
   ============================================ */
#define GAIN_MIN            0.8f        // Gain tối thiểu
#define GAIN_MAX            10.0f       // Gain tối đa

/* ============================================
   CẤU HÌNH PHẦN CỨNG
   ============================================ */
#define NUMBER_OF_LED       80          // Số LED trên VU-meter

/* ============================================
   CẤU HÌNH ADC (Analog-to-Digital Converter)
   ============================================ */
#define AUDIO_ADC_CHANNEL       0       // ADC_IN0 (PC0) - Tín hiệu audio
#define GAIN_ADC_CHANNEL        1       // ADC_IN1 (PC1) - Điều chỉnh gain
#define THRESHOLD_ADC_CHANNEL   2       // ADC_IN2 (PC2) - Ngưỡng

/* ADC tương ứng với port */
#define ADC_PORT            GPIOC
#define ADC_AUDIO_PIN       GPIO_Pin_0  // PC0
#define ADC_GAIN_PIN        GPIO_Pin_1  // PC1
#define ADC_THRESHOLD_PIN   GPIO_Pin_2  // PC2

/* ============================================
   CẤU HÌNH SPI (Shift Register HC595)
   ============================================ */
#define HC595_SPI           SPI1

/* Chân SPI */
#define HC595_MOSI_PORT     GPIOC
#define HC595_MOSI_PIN      GPIO_Pin_6  // PC6 (SPI1_MOSI)

#define HC595_SCK_PORT      GPIOC
#define HC595_SCK_PIN       GPIO_Pin_5  // PC5 (SPI1_SCK)

#define HC595_CS_PORT       GPIOC
#define HC595_CS_PIN        GPIO_Pin_4  // PC4 (Chip Select - GPIO)

/* Chân điều khiển HC595 */
#define HC595_OE_PORT       GPIOA
#define HC595_OE_PIN        GPIO_Pin_3  // PA3 (Output Enable - active LOW)

#define HC595_SCL_PORT      GPIOA
#define HC595_SCL_PIN       GPIO_Pin_6  // PA6 (Serial Clear - active LOW)

/* ============================================
   CẤU HÌNH UART (Debug via Serial)
   ============================================ */
#define UART_DEBUG          1           // 1 = bật debug, 0 = tắt

#define DEBUG_USART         USART1
#define DEBUG_BAUDRATE      115200

#define DEBUG_TX_PORT       GPIOD
#define DEBUG_TX_PIN        GPIO_Pin_5  // PD5 (USART1_TX)

#define DEBUG_RX_PORT       GPIOD
#define DEBUG_RX_PIN        GPIO_Pin_6  // PD6 (USART1_RX)

/* ============================================
   CẤU HÌNH TIMER (Lấy mẫu ADC)
   ============================================ */
#define SAMPLING_TIMER      TIM1        // Timer cho ADC trigger
#define SAMPLING_FREQ       10000       // 10 kHz (100 µs)

/* ============================================
   CẤU HÌNH WATCHDOG
   ============================================ */
#define WATCHDOG_ENABLED    0           // 1 = bật, 0 = tắt

#endif /* CONFIG_H_ */
