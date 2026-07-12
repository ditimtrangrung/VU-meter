/**
 * @file spi.h
 * @brief Header file cho SPI và HC595 Shift Register
 * @author ditimtrangrung
 *
 * Điều khiển LED qua SPI + 74HC595 shift register
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

/**
 * @fn void SPI_Init(void)
 * @brief Khởi tạo SPI1 để giao tiếp với HC595
 *
 * Cấu hình:
 * - SPI1 Master mode
 * - 8-bit data
 * - Baudrate: 3 MHz (Prescaler = 16)
 * - MOSI trên PC6, SCK trên PC5
 */
void SPI_Init(void);

/**
 * @fn void SPI_SendByte(uint8_t data)
 * @brief Gửi 1 byte qua SPI
 *
 * @param data Byte cần gửi
 */
void SPI_SendByte(uint8_t data);

/**
 * @fn void HC595_Init(void)
 * @brief Khởi tạo HC595 shift register
 *
 * Cấu hình chân điều khiển:
 * - CS (PC4): Chip select
 * - OE (PA3): Output enable (active LOW)
 * - SCL (PA6): Serial clear (active LOW)
 */
void HC595_Init(void);

/**
 * @fn void HC595_Write(uint8_t vu_value, uint8_t peak_value)
 * @brief Ghi dữ liệu đèn LED vào HC595
 *
 * Byte 1: VU-meter (bit 0-7 = LED 0-7)
 * Byte 2: VU-meter tiếp (bit 0-7 = LED 8-15)
 * ...
 * Byte 10: Peak indicator
 *
 * @param vu_value Mức VU-meter (0-80)
 * @param peak_value Mức peak (0-80)
 */
void HC595_Write(uint8_t vu_value, uint8_t peak_value);

/**
 * @fn void HC595_Clear(void)
 * @brief Tắt tất cả LED (xóa HC595)
 */
void HC595_Clear(void);

#endif /* SPI_H_ */
