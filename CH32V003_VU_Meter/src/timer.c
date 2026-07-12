/**
 * @file timer.c
 * @brief Chương trình Timer cho CH32V003
 * @author ditimtrangrung
 */

#include "ch32v00x.h"
#include "timer.h"
#include "config.h"

/**
 * @brief Khởi tạo Timer1 cho ADC sampling
 */
void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // Bật clock cho Timer1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // Tính toán:
    // Clock: 48 MHz
    // Prescaler: 47 (tức là 48MHz / 48 = 1 MHz)
    // Period: 100 (tức là 1MHz / 100 = 10kHz = 100µs)
    
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;        // 100µs
    TIM_TimeBaseInitStructure.TIM_Prescaler = 48 - 1;      // 1MHz clock
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    // Cấu hình Output Compare (CC1) để trigger ADC
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 50;                     // 50% duty
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);

    // Cấu hình NVIC cho Timer1 interrupt
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Bật interrupt
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    // Khởi tạo Timer nhưng chưa chạy
    TIM_Cmd(TIM1, DISABLE);
}

/**
 * @brief Bắt đầu Timer1
 */
void Timer_Start(void)
{
    TIM_Cmd(TIM1, ENABLE);
}

/**
 * @brief Dừng Timer1
 */
void Timer_Stop(void)
{
    TIM_Cmd(TIM1, DISABLE);
}
