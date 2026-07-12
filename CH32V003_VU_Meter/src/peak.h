/**
 * @file peak.h
 * @brief Header file cho Peak Hold kiểu chuyên nghiệp
 * @author ditimtrangrung
 *
 * Peak Hold kỹ thuật:
 * - Attack Time: ~5ms (nhạy cảm cao với spike)
 * - Hold Time: 300-500ms (giữ peak không đổi)
 * - Release Time: 300ms-1s (rơi từng từ)
 */

#ifndef PEAK_H_
#define PEAK_H_

#include <stdint.h>

/* Peak Hold parameters */
#define PEAK_ATTACK_TIME    5           // ms (instant - nhạy cảm với spike)
#define PEAK_HOLD_TIME_MS   300         // ms (giữ peak)
#define PEAK_RELEASE_TIME   300         // ms (thời gian rơi từng từ)
#define PEAK_RELEASE_RATE   0.2f        // LED/step (rơi 0.2 LED mỗi 10ms)

/**
 * @struct Peak_Object
 * @brief Object lưu trữ trạng thái peak
 */
typedef struct {
    uint8_t peak_value;         /**< Giá trị peak hiện tại */
    uint16_t hold_counter;      /**< Bộ đếm hold time */
    float peak_float;           /**< Peak dạng float để release mượt */
    uint8_t is_holding;         /**< 1 = đang giữ, 0 = đang release */
} Peak_Object;

/**
 * @fn void Peak_Init(Peak_Object *peak)
 * @brief Khởi tạo peak detector
 *
 * @param peak Con trỏ đến Peak_Object
 */
void Peak_Init(Peak_Object *peak);

/**
 * @fn void Peak_Update(Peak_Object *peak, uint8_t current_level)
 * @brief Cập nhật peak với Attack/Hold/Release
 *
 * @param peak Con trỏ đến Peak_Object
 * @param current_level Mức hiện tại (0-80)
 */
void Peak_Update(Peak_Object *peak, uint8_t current_level);

#endif /* PEAK_H_ */
