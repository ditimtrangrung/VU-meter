/**
 * @file peak.c
 * @brief Triển khai Peak Hold chuyên nghiệp
 * @author ditimtrangrung
 *
 * Mô phỏng peak meter thiết bị âm thanh:
 * - Giữ peak level khi có signal
 * - Hold time trước khi bắt đầu decay
 * - Release mượt mà theo PEAK_RELEASE_RATE
 */

#include "peak.h"
#include "config.h"

/**
 * @brief Khởi tạo peak detector
 */
void Peak_Init(Peak_Object *peak)
{
    peak->peak_value = 0;
    peak->hold_counter = 0;
    peak->peak_float = 0.0f;
    peak->is_holding = 0;
}

/**
 * @brief Cập nhật peak với Attack/Hold/Release
 *
 * Các giai đoạn:
 * 1. ATTACK: current_level > peak_value → cập nhật peak ngay lập tức
 * 2. HOLD: peak_value giữ nguyên, bộ đếm hold
 * 3. RELEASE: Sau hold_time, peak giảm dần
 */
void Peak_Update(Peak_Object *peak, uint8_t current_level)
{
    // === ATTACK ===
    // Nếu có signal cao hơn peak, cập nhật ngay (attack instant)
    if(current_level > peak->peak_value)
    {
        peak->peak_value = current_level;
        peak->peak_float = (float)current_level;
        peak->hold_counter = PEAK_HOLD_TIME_MS / 10;  // 10ms per update
        peak->is_holding = 1;
        return;
    }

    // === HOLD ===
    // Giữ peak value, giảm hold counter
    if(peak->is_holding && peak->hold_counter > 0)
    {
        peak->hold_counter--;
        return;
    }

    // === RELEASE ===
    // Sau hold time, peak giảm dần mượt mà
    if(peak->peak_float > 0)
    {
        peak->peak_float -= PEAK_RELEASE_RATE;
        
        if(peak->peak_float < 0) {
            peak->peak_float = 0;
        }
        
        peak->peak_value = (uint8_t)peak->peak_float;
        peak->is_holding = 0;
    }
    else
    {
        peak->peak_value = 0;
        peak->is_holding = 0;
    }
}
