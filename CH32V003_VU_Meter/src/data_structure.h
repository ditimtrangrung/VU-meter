/**
 * @file data_structure.h
 * @brief Định nghĩa các cấu trúc dữ liệu cho VU-meter
 * @author ditimtrangrung
 *
 * Tệp này định nghĩa các struct dùng để lưu trữ dữ liệu audio
 */

#ifndef DATA_STRUCTURE_H_
#define DATA_STRUCTURE_H_

#include <stdint.h>

/**
 * @struct audio_object
 * @brief Cấu trúc lưu trữ dữ liệu audio
 *
 * Chứa giá trị trung bình ADC, mức VU-meter, peak level
 */
typedef struct {
    uint16_t adc_avg;       /**< Giá trị ADC trung bình */
    uint8_t vu_value;       /**< Mức VU-meter (0-80 LED) */
    uint8_t peak_value;     /**< Mức peak (0-80 LED) */
    uint16_t peak_counter;  /**< Bộ đếm thời gian giữ peak */
} audio_object;

#endif /* DATA_STRUCTURE_H_ */
