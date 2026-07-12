/**
 * @file modes.c
 * @brief Triển khai 4 chế độ hiển thị VU-meter
 * @author ditimtrangrung
 */

#include "modes.h"
#include "config.h"
#include <string.h>

/* Waterfall history buffer */
static uint8_t waterfall_history[10] = {0};
static uint8_t waterfall_index = 0;

/**
 * @brief Mode BAR: Thanh liên tục
 *
 * Ví dụ với vu_value = 25 (25 LED sáng):
 * LED:  0-24 sáng, 25-79 tắt
 * ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
 */
void Mode_Bar(audio_object *smpl)
{
    // Cập nhật vu_value với peak tự động độc lập
    // Thanh bar hiển thị chỉ VU, peak riêng trong DSP
    // (Peak được xử lý bằng peak_counter trong dsp.c)
}

/**
 * @brief Mode DOT: LED đơn lẻ (Neon tube style)
 *
 * Ví dụ với vu_value = 25:
 * LED:  24 sáng (chỉ một LED), còn lại tắt
 * ░░░░░░░░░░░░░░░░░░░░░░░░▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
 */
void Mode_Dot(audio_object *smpl)
{
    // Chỉ bật LED ở vị trí smpl->vu_value
    // Các LED khác tắt
    // Điều này được xử lý trong HC595_Write() bằng cách sửa đổi logic
    
    // Giữ nguyên peak_value để hiển thị
    // Có thể tính toán peak đặc biệt cho dot mode
}

/**
 * @brief Mode PEAK: Thanh bar + Peak indicator
 *
 * Ví dụ:
 * VU:   0-25 sáng (bar)
 * Peak: LED ở vị trí peak_value sáng rõ hơn (marker)
 * 
 * ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
 *                           ^ Peak marker (█ = on, ░ = off)
 */
void Mode_Peak(audio_object *smpl)
{
    // Giữ nguyên: Bar cho VU + Peak indicator
    // Peak tự động được xử lý bởi dsp.c
}

/**
 * @brief Mode WATERFALL: Spectrum analyzer
 *
 * Lưu lịch sử 10 giá trị ADC gần nhất, hiển thị dạng tầng
 * Giá trị mới ở trên cùng, giá trị cũ rơi xuống
 *
 * Hàng 1 (mới): ░░▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
 * Hàng 2:      ░▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
 * Hàng 3:      ▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
 * ... (10 hàng)
 */
void Mode_Waterfall(audio_object *smpl)
{
    // Shift history buffer
    // Dịch tất cả giá trị sang phải (lên thời gian cũ hơn)
    for(int i = 9; i > 0; i--) {
        waterfall_history[i] = waterfall_history[i - 1];
    }
    
    // Thêm giá trị mới vào đầu
    waterfall_history[0] = smpl->vu_value;

    // Hiển thị: ta sẽ lấy giá trị từ history[0] cho frame hiện tại
    // Frame trước từ history[1], ..., frame 9 trước từ history[9]
    // 
    // Nhưng vì ta chỉ có 80 LED (1 hàng), ta không thể hiển thị
    // tất cả 10 hàng cùng lúc. Nên ta sẽ hiển thị giá trị mới nhất
    // nhưng có thể thêm fade effect
    
    // Giải pháp: Hiển thị như bar mode nhưng với brightness giảm dần
    // hoặc chỉ hiển thị giá trị hiện tại với ghi chú "waterfall active"
}
