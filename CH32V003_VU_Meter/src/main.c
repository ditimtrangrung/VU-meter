/**
 * @file main.c
 * @brief Chương trình chính cho VU-meter CH32V003F4P6
 * @author ditimtrangrung
 *
 * Tính năng:
 * - 4 chế độ hiển thị: Bar, Dot, Peak, Waterfall (lựa chọn qua jumper PA0-PA2)
 * - AGC (Automatic Gain Control) tự động
 * - Peak Hold kiểu chuyên nghiệp (Attack/Release time)
 * - UART debug
 */

#ifndef F_CPU
#define F_CPU 48000000UL  // 48 MHz
#endif

#include "ch32v00x.h"
#include <string.h>
#include "config.h"
#include "adc.h"
#include "spi.h"
#include "uart.h"
#include "timer.h"
#include "dsp.h"
#include "data_structure.h"
#include "modes.h"
#include "agc.h"

/* ============================================
   GLOBAL VARIABLES
   ============================================ */

audio_object audio_sample = {0, 0, 0, 0};
int data_smpl_counter = 0;
float audio_avg = 0;
float current_gain = 1.0f;
uint16_t threshold = 30;
uint8_t sample_ready = 0;
uint8_t adc_channel = 0;
uint16_t adc_raw = 0;

/* Mode selection: 0=Bar, 1=Dot, 2=Peak, 3=Waterfall */
uint8_t current_mode = 0;

/* AGC */
AGC_Object agc;

/* ============================================
   FUNCTION PROTOTYPES
   ============================================ */

void GPIO_Init_Custom(void);
void ReadModeSelector(void);
void SystemInit_Custom(void);

/* ============================================
   MAIN PROGRAM
   ============================================ */

int main(void)
{
    SystemInit_Custom();

    #if UART_DEBUG
    UART_Init(DEBUG_BAUDRATE);
    UART_SendString("\r\n=== VU-Meter CH32V003 ===\r\n");
    UART_SendString("Khoi dong he thong...\r\n");
    #endif

    // Khởi tạo các ngoại vi
    GPIO_Init_Custom();
    Timer_Init();
    ADC_Init();
    HC595_Init();
    AGC_Init(&agc);

    // Khởi tạo dữ liệu audio
    audio_sample.vu_value = 0;
    audio_sample.peak_value = 0;
    audio_sample.peak_counter = 0;
    audio_sample.adc_avg = 0;

    #if UART_DEBUG
    UART_SendString("Cac ngoai vi da khoi dong\r\n");
    #endif

    // Bật interrupt toàn cục
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    __enable_irq();

    // Bắt đầu Timer
    Timer_Start();

    // Vòng lặp chính
    while(1)
    {
        // Đọc mode từ jumper PA0-PA2
        ReadModeSelector();

        // Nếu có mẫu sẵn sàng
        if(sample_ready)
        {
            sample_ready = 0;

            // === AGC Processing ===
            AGC_Process(&agc, audio_sample.adc_avg);
            current_gain = agc.current_gain;

            // === DSP Processing (tính VU + Peak) ===
            dsp_VU_peak_compute(&audio_sample);

            // === Ghi dữ liệu LED theo chế độ hiển thị ===
            switch(current_mode)
            {
                case MODE_BAR:
                    Mode_Bar(&audio_sample);
                    break;
                case MODE_DOT:
                    Mode_Dot(&audio_sample);
                    break;
                case MODE_PEAK:
                    Mode_Peak(&audio_sample);
                    break;
                case MODE_WATERFALL:
                    Mode_Waterfall(&audio_sample);
                    break;
                default:
                    Mode_Bar(&audio_sample);
            }

            // Cập nhật LED
            HC595_Write(audio_sample.vu_value, audio_sample.peak_value);

            #if UART_DEBUG
            if(adc_channel & 1) {
                UART_Printf("Mode:%d ADC:%4d VU:%3d Peak:%3d Gain:%.2f\r\n",
                    current_mode, audio_sample.adc_avg, 
                    audio_sample.vu_value, audio_sample.peak_value,
                    current_gain);
            }
            #endif

            // Chuyển đổi kênh ADC
            if(adc_channel & 1) {
                ADC_RegularChannelConfig(ADC1, GAIN_ADC_CHANNEL, 1, ADC_SampleTime_239Cycles);
            } else {
                ADC_RegularChannelConfig(ADC1, THRESHOLD_ADC_CHANNEL, 1, ADC_SampleTime_239Cycles);
            }
            adc_channel++;
        }

        // Delay nhẹ
        for(volatile int i = 0; i < 100; i++);
    }

    return 0;
}

/* ============================================
   GPIO INITIALIZATION
   ============================================ */

/**
 * @brief Khởi tạo GPIO cho Mode Selector (PA0-PA2)
 */
void GPIO_Init_Custom(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Bật clock cho GPIOA
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);

    // PA0, PA1, PA2 = Input (mode selector jumper)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @brief Đọc mode từ jumper PA0-PA2
 */
void ReadModeSelector(void)
{
    // Đọc 3 bit từ PA0, PA1, PA2
    uint8_t mode_bits = 0;

    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) mode_bits |= 0x01;
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)) mode_bits |= 0x02;
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)) mode_bits |= 0x04;

    // Lấy 2 bit thấp nhất (00-11)
    current_mode = mode_bits & 0x03;
}

/**
 * @brief Khởi tạo hệ thống tùy chỉnh
 */
void SystemInit_Custom(void)
{
    // Gọi SystemInit từ startup
    SystemInit();
}

/* ============================================
   INTERRUPT HANDLERS
   ============================================ */

/**
 * @brief ADC Interrupt Handler
 */
void ADC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void ADC_IRQHandler(void)
{
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC))
    {
        adc_raw = ADC_GetConversionValue(ADC1);
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);

        // Xử lý dữ liệu ADC
        if((ADMUX & 0x0F) == AUDIO_ADC_CHANNEL)
        {
            // Đang đọc tín hiệu audio
            adc_raw = (float)adc_raw * current_gain;
            if(adc_raw > 1023) adc_raw = 1023;

            if(adc_raw < threshold) adc_raw = threshold;

            audio_avg += adc_raw;
            data_smpl_counter++;

            // Down-sample từ 10kHz xuống 100Hz
            if(data_smpl_counter == AVERAGEWIDTH)
            {
                audio_avg /= AVERAGEWIDTH;
                audio_avg = (audio_avg - threshold) * 1023 / (1023 - threshold);

                audio_sample.adc_avg = (uint16_t)audio_avg;
                sample_ready = 1;

                audio_avg = 0;
                data_smpl_counter = 0;
            }
        }
    }
}

/**
 * @brief Timer1 Update Interrupt Handler
 */
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        // Trigger ADC conversion
        ADC_StartConversion();
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

/**
 * @brief UART1 Interrupt Handler
 */
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART1);
        
        // Có thể xử lý lệnh từ serial tại đây
        (void)data;  // Tránh warning
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
