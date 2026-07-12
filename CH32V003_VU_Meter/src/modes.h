/**
 * @file modes.h
 * @brief Header file cho 4 chế độ hiển thị
 * @author ditimtrangrung
 *
 * - MODE_BAR (00): Thanh LED liên tục từ trái sang phải
 * - MODE_DOT (01): LED đơn lẻ (dot/neon)
 * - MODE_PEAK (10): Thanh + Peak indicator
 * - MODE_WATERFALL (11): Waterfall/spectrum analyzer
 */

#ifndef MODES_H_
#define MODES_H_

#include "data_structure.h"

/* Mode definitions */
#define MODE_BAR        0
#define MODE_DOT        1
#define MODE_PEAK       2
#define MODE_WATERFALL  3

/**
 * @fn void Mode_Bar(audio_object *smpl)
 * @brief Chế độ Bar: Thanh LED liên tục từ 0 đến vu_value
 * 
 * Tất cả LED từ 0 đến N sáng (N = vu_value)
 * 
 * @param smpl Struct audio_object
 */
void Mode_Bar(audio_object *smpl);

/**
 * @fn void Mode_Dot(audio_object *smpl)
 * @brief Chế độ Dot: Chỉ LED ở vị trí vu_value sáng
 *
 * Giống neon tube indicator, chỉ 1 LED sáng
 *
 * @param smpl Struct audio_object
 */
void Mode_Dot(audio_object *smpl);

/**
 * @fn void Mode_Peak(audio_object *smpl)
 * @brief Chế độ Peak: Thanh LED + Peak indicator ở đầu
 *
 * Kết hợp chế độ bar với peak nhảy ra phía trước
 *
 * @param smpl Struct audio_object
 */
void Mode_Peak(audio_object *smpl);

/**
 * @fn void Mode_Waterfall(audio_object *smpl)
 * @brief Chế độ Waterfall: Spectrum analyzer dạng tầng
 *
 * LED hiện tại ở hàng trên, các giá trị cũ rơi xuống (hold time)
 *
 * @param smpl Struct audio_object
 */
void Mode_Waterfall(audio_object *smpl);

#endif /* MODES_H_ */
