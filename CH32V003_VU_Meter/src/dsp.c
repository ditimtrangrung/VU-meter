/**
 * @file dsp.c
 * @brief Chương trình xử lý tín hiệu (DSP)
 * @author ditimtrangrung
 *
 * Thực hiện các phép toán DSP:
 * - Ánh xạ giá trị ADC sang mức LED
 * - Giữ peak và decay dần theo thời gian
 */

#include "dsp.h"
#include "config.h"

/**
 * @brief Tính toán mức VU và peak
 *
 * Chuyển đổi giá trị ADC (0-1023) thành LED index (0-80)
 * Xử lý peak detection với thời gian decay
 *
 * @param smpl Struct audio_object
 */
void dsp_VU_peak_compute(audio_object *smpl)
{
    // ===== Tính VU-meter =====
    // Ánh xạ ADC_avg (0-1023) sang vu_value (0-80)
    uint8_t vu_calc = (smpl->adc_avg * NUMBER_OF_LED) / 1024;
    if (vu_calc > NUMBER_OF_LED) vu_calc = NUMBER_OF_LED;
    
    smpl->vu_value = vu_calc;
    
    // ===== Peak Detection =====
    // Nếu VU hiện tại cao hơn peak, cập nhật peak
    if (vu_calc > smpl->peak_value) {
        smpl->peak_value = vu_calc;
        smpl->peak_counter = PEAK_HOLD_TIME; // Reset hold time
    }
    
    // ===== Peak Decay =====
    // Giảm dần peak theo thời gian
    if (smpl->peak_counter > 0) {
        smpl->peak_counter--;
    } else {
        // Giảm peak dần
        if (smpl->peak_value > 0) {
            smpl->peak_value = (uint8_t)((float)smpl->peak_value - PEAK_FALL_RATE);
            if (smpl->peak_value < 0) smpl->peak_value = 0;
        }
    }
}
