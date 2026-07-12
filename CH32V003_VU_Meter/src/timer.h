/**
 * @file timer.h
 * @brief Header file cho Timer
 * @author ditimtrangrung
 *
 * Khởi tạo Timer1 để trigger ADC sampling
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

/**
 * @fn void Timer_Init(void)
 * @brief Khởi tạo Timer1 với tần số sampling 10kHz
 *
 * Timer1 trigger ADC mỗi 100µs (10kHz)
 */
void Timer_Init(void);

/**
 * @fn void Timer_Start(void)
 * @brief Bắt đầu Timer1
 */
void Timer_Start(void);

/**
 * @fn void Timer_Stop(void)
 * @brief Dừng Timer1
 */
void Timer_Stop(void);

#endif /* TIMER_H_ */
