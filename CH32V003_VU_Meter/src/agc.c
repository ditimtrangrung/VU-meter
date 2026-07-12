/**
 * @file agc.c
 * @brief Triển khai AGC (Automatic Gain Control)
 * @author ditimtrangrung
 *
 * Cơ chế:
 * 1. Tính target gain: Nếu level > target, giảm gain; nếu < target, tăng gain
 * 2. Áp dụng attack/release smoothing
 * 3. Giới hạn gain trong [AGC_MIN_GAIN, AGC_MAX_GAIN]
 */

#include "agc.h"
#include "config.h"
#include <math.h>

/**
 * @brief Khởi tạo AGC
 */
void AGC_Init(AGC_Object *agc)
{
    agc->current_gain = 1.0f;
    agc->target_gain = 1.0f;
    
    // Tính attack coefficient (10% gain change per step khi attack)
    // Attack time = 0.1s ở 100Hz = 10 steps
    // Change per step = 1 / 10 = 0.1
    agc->attack_coefficient = 1.0f / (AGC_ATTACK_TIME * AGC_SAMPLE_RATE);
    
    // Release coefficient (1% gain change per step khi release)
    // Release time = 1.0s ở 100Hz = 100 steps
    // Change per step = 1 / 100 = 0.01
    agc->release_coefficient = 1.0f / (AGC_RELEASE_TIME * AGC_SAMPLE_RATE);
}

/**
 * @brief Xử lý AGC
 *
 * Công thức:
 * 1. target_gain = AGC_TARGET_LEVEL / level (nếu level > 0)
 * 2. Nếu target_gain > current_gain: attack (tăng nhanh)
 * 3. Nếu target_gain < current_gain: release (giảm chậm)
 */
void AGC_Process(AGC_Object *agc, uint16_t level)
{
    // Tránh division by zero
    if(level == 0) level = 1;

    // Tính gain mục tiêu
    agc->target_gain = (float)AGC_TARGET_LEVEL / (float)level;

    // Giới hạn target_gain
    if(agc->target_gain > AGC_MAX_GAIN) {
        agc->target_gain = AGC_MAX_GAIN;
    }
    if(agc->target_gain < AGC_MIN_GAIN) {
        agc->target_gain = AGC_MIN_GAIN;
    }

    // Áp dụng attack/release smoothing
    if(agc->target_gain > agc->current_gain)
    {
        // Attack: Tăng gain nhanh
        float delta = (agc->target_gain - agc->current_gain) * agc->attack_coefficient;
        agc->current_gain += delta;
        
        if(agc->current_gain > agc->target_gain) {
            agc->current_gain = agc->target_gain;
        }
    }
    else if(agc->target_gain < agc->current_gain)
    {
        // Release: Giảm gain chậm
        float delta = (agc->current_gain - agc->target_gain) * agc->release_coefficient;
        agc->current_gain -= delta;
        
        if(agc->current_gain < agc->target_gain) {
            agc->current_gain = agc->target_gain;
        }
    }
}
