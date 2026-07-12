/**
 * @file adc.h
 * @brief Header file cho ADC (Analog-to-Digital Converter)
 * @author ditimtrangrung
 *
 * Khởi tạo và điều khiển ADC trên CH32V003
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

/**
 * @fn void ADC_Init(void)
 * @brief Khởi tạo ADC1 trên CH32V003
 *
 * Cấu hình:
 * - 3 kênh input: PC0 (Audio), PC1 (Gain), PC2 (Threshold)
 * - 10-bit resolution
 * - Độc lập, không scan
 * - Trigger từ Timer1 (CC1)
 */
void ADC_Init(void);

/**
 * @fn uint16_t ADC_GetValue(uint8_t channel)
 * @brief Đọc giá trị từ kênh ADC cụ thể
 *
 * @param channel Kênh ADC (0-7)
 * @return Giá trị ADC 10-bit (0-1023)
 */
uint16_t ADC_GetValue(uint8_t channel);

/**
 * @fn void ADC_StartConversion(void)
 * @brief Bắt đầu chuyển đổi ADC
 */
void ADC_StartConversion(void);

/**
 * @fn uint16_t ADC_GetConvertedValue(void)
 * @brief Lấy giá trị đã chuyển đổi
 * @return Giá trị ADC
 */
uint16_t ADC_GetConvertedValue(void);

#endif /* ADC_H_ */
