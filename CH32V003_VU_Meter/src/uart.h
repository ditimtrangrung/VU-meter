/**
 * @file uart.h
 * @brief Header file cho UART Debug
 * @author ditimtrangrung
 *
 * Giao tiếp serial UART1 để debug
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

/**
 * @fn void UART_Init(uint32_t baudrate)
 * @brief Khởi tạo UART1
 *
 * @param baudrate Tốc độ baud (ví dụ: 115200)
 */
void UART_Init(uint32_t baudrate);

/**
 * @fn void UART_SendByte(uint8_t data)
 * @brief Gửi 1 byte qua UART
 *
 * @param data Byte cần gửi
 */
void UART_SendByte(uint8_t data);

/**
 * @fn void UART_SendString(const char *str)
 * @brief Gửi chuỗi qua UART
 *
 * @param str Con trỏ đến chuỗi
 */
void UART_SendString(const char *str);

/**
 * @fn void UART_Printf(const char *fmt, ...)
 * @brief In format string qua UART (giống printf)
 *
 * @param fmt Format string
 */
void UART_Printf(const char *fmt, ...);

/**
 * @fn uint8_t UART_GetByte(void)
 * @brief Nhận 1 byte từ UART (chặn)
 *
 * @return Byte nhận được
 */
uint8_t UART_GetByte(void);

#endif /* UART_H_ */
