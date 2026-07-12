/**
 * @file adc.c
 * @brief Chương trình ADC cho CH32V003
 * @author ditimtrangrung
 *
 * Thực hiện khởi tạo và điều khiển ADC
 */

#include "ch32v00x.h"
#include "adc.h"
#include "config.h"

/**
 * @brief Khởi tạo ADC1 trên CH32V003
 */
void ADC_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // Bật clock cho GPIOC và ADC1
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_ADC1, ENABLE);

    // Cấu hình chân PC0, PC1, PC2 ở chế độ analog input
    GPIO_InitStructure.GPIO_Pin = ADC_AUDIO_PIN | ADC_GAIN_PIN | ADC_THRESHOLD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC_PORT, &GPIO_InitStructure);

    // Reset ADC1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);

    // Cấu hình ADC1
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;  // Trigger từ Timer1 CC1
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Bật ADC1
    ADC_Cmd(ADC1, ENABLE);

    // Calibrate ADC1
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    // Cấu hình NVIC cho ADC interrupt (tuỳ chọn)
    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Bật interrupt cho ADC
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}

/**
 * @brief Đọc giá trị từ kênh ADC cụ thể
 */
uint16_t ADC_GetValue(uint8_t channel)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

/**
 * @brief Bắt đầu chuyển đổi ADC (không chặn)
 */
void ADC_StartConversion(void)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
 * @brief Lấy giá trị đã chuyển đổi
 */
uint16_t ADC_GetConvertedValue(void)
{
    return ADC_GetConversionValue(ADC1);
}
