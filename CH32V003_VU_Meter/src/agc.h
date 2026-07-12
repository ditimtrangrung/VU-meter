/**
 * @file agc.h
 * @brief Header file cho AGC (Automatic Gain Control)
 * @author ditimtrangrung
 *
 * Tự động điều chỉnh gain dựa trên mức tín hiệu input
 * Target level: 800 (80% of 1024)
 * Attack time: nhanh (0.1s)
 * Release time: chậm (1.0s)
 */

#ifndef AGC_H_
#define AGC_H_

#include <stdint.h>

/* AGC constants */
#define AGC_TARGET_LEVEL    800         // Target output level
#define AGC_MAX_GAIN        10.0f       // Maximum gain
#define AGC_MIN_GAIN        0.5f        // Minimum gain
#define AGC_ATTACK_TIME     0.1f        // Attack time (seconds)
#define AGC_RELEASE_TIME    1.0f        // Release time (seconds)
#define AGC_SAMPLE_RATE     100         // AGC update rate (100Hz)

/**
 * @struct AGC_Object
 * @brief Object chứa trạng thái AGC
 */
typedef struct {
    float current_gain;         /**< Gain hiện tại */
    float target_gain;          /**< Gain mục tiêu */
    float attack_coefficient;   /**< Coefficient cho attack */
    float release_coefficient;  /**< Coefficient cho release */
} AGC_Object;

/**
 * @fn void AGC_Init(AGC_Object *agc)
 * @brief Khởi tạo AGC
 *
 * @param agc Con trỏ đến AGC_Object
 */
void AGC_Init(AGC_Object *agc);

/**
 * @fn void AGC_Process(AGC_Object *agc, uint16_t level)
 * @brief Xử lý AGC
 *
 * Tính toán gain mục tiêu dựa trên level input
 * Áp dụng attack/release time để smoothing
 *
 * @param agc Con trỏ đến AGC_Object
 * @param level Mức tín hiệu input (0-1023)
 */
void AGC_Process(AGC_Object *agc, uint16_t level);

#endif /* AGC_H_ */
