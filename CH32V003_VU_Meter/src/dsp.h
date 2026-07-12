/**
 * @file dsp.h
 * @brief Header file cho Digital Signal Processing
 * @author ditimtrangrung
 *
 * Định nghĩa các hàm xử lý tín hiệu DSP
 */

#ifndef DSP_H_
#define DSP_H_

#include "data_structure.h"

/**
 * @fn void dsp_VU_peak_compute(audio_object * smpl)
 * @brief Tính toán giá trị VU-meter và peak level
 *
 * Hàm này nhận giá trị ADC trung bình, tính toán mức VU-meter
 * và xử lý peak detection với time decay
 *
 * @param smpl Con trỏ đến struct audio_object chứa dữ liệu ADC
 * @return Không
 */
void dsp_VU_peak_compute(audio_object *smpl);

#endif /* DSP_H_ */
