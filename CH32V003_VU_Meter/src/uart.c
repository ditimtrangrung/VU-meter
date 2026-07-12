/**
 * @file uart.c
 * @brief Chương trình UART cho CH32V003
 * @author ditimtrangrung
 */

#include "ch32v00x.h"
#include "uart.h"
#include "config.h"
#include <stdio.h>
#include <stdarg.h>

static char printf_buffer[256];

/**
 * @brief Khởi tạo UART1
 */
void UART_Init(uint32_t baudrate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // Bật clock cho GPIOD và USART1
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOD, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_USART1, ENABLE);

    // Cấu hình TX (PD5) ở chế độ Alternate Function Push-Pull
    GPIO_InitStructure.GPIO_Pin = DEBUG_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DEBUG_TX_PORT, &GPIO_InitStructure);

    // Cấu hình RX (PD6) ở chế độ Input Floating
    GPIO_InitStructure.GPIO_Pin = DEBUG_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_RX_PORT, &GPIO_InitStructure);

    // Cấu hình USART1
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(DEBUG_USART, &USART_InitStructure);

    // Bật interrupt RX (tuỳ chọn)
    USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);

    // Cấu hình NVIC
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Bật USART1
    USART_Cmd(DEBUG_USART, ENABLE);
}

/**
 * @brief Gửi 1 byte
 */
void UART_SendByte(uint8_t data)
{
    while(USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TC) == RESET);
    USART_SendData(DEBUG_USART, data);
}

/**
 * @brief Gửi chuỗi
 */
void UART_SendString(const char *str)
{
    while(*str) {
        UART_SendByte(*str++);
    }
}

/**
 * @brief Printf format
 */
void UART_Printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsprintf(printf_buffer, fmt, args);
    va_end(args);
    UART_SendString(printf_buffer);
}

/**
 * @brief Nhận 1 byte (chặn)
 */
uint8_t UART_GetByte(void)
{
    while(USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(DEBUG_USART);
}
