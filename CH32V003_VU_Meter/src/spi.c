/**
 * @file spi.c
 * @brief Chương trình SPI và HC595 cho CH32V003
 * @author ditimtrangrung
 */

#include "ch32v00x.h"
#include "spi.h"
#include "config.h"
#include <string.h>

// Buffer lưu trữ trạng thái 10 bytes HC595 (80 LED / 8 = 10 bytes)
static uint8_t led_buffer[10];

/**
 * @brief Khởi tạo SPI1
 */
void SPI_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // Bật clock cho GPIOC, GPIOA và SPI1
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC | RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_SPI1, ENABLE);

    // Cấu hình MOSI (PC6) và SCK (PC5) ở chế độ Alternate Function Push-Pull
    GPIO_InitStructure.GPIO_Pin = HC595_MOSI_PIN | HC595_SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HC595_MOSI_PORT, &GPIO_InitStructure);

    // Cấu hình CS (PC4) ở chế độ GPIO output
    GPIO_InitStructure.GPIO_Pin = HC595_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HC595_CS_PORT, &GPIO_InitStructure);

    // Cấu hình OE (PA3) và SCL (PA6) ở chế độ GPIO output
    GPIO_InitStructure.GPIO_Pin = HC595_OE_PIN | HC595_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HC595_OE_PORT, &GPIO_InitStructure);

    // Cấu hình SPI1
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  // Chỉ Tx
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  // Software controlled
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;  // 48MHz/16 = 3MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(HC595_SPI, &SPI_InitStructure);

    // Bật SPI1
    SPI_Cmd(HC595_SPI, ENABLE);

    // Khởi tạo trạng thái chân
    GPIO_SetBits(HC595_CS_PORT, HC595_CS_PIN);      // CS cao (không chọn)
    GPIO_ResetBits(HC595_OE_PORT, HC595_OE_PIN);    // OE thấp (bật LED)
    GPIO_SetBits(HC595_SCL_PORT, HC595_SCL_PIN);    // SCL cao (không xóa)
}

/**
 * @brief Gửi 1 byte qua SPI
 */
void SPI_SendByte(uint8_t data)
{
    while(SPI_I2S_GetFlagStatus(HC595_SPI, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(HC595_SPI, data);
    while(SPI_I2S_GetFlagStatus(HC595_SPI, SPI_I2S_FLAG_BSY) == SET);
}

/**
 * @brief Khởi tạo HC595
 */
void HC595_Init(void)
{
    // Gọi SPI_Init() trước
    SPI_Init();
    
    // Xóa buffer LED
    memset(led_buffer, 0, sizeof(led_buffer));
    
    // Reset HC595 shift register
    GPIO_ResetBits(HC595_SCL_PORT, HC595_SCL_PIN);  // SCL thấp (reset)
    for(volatile int i = 0; i < 100; i++);
    GPIO_SetBits(HC595_SCL_PORT, HC595_SCL_PIN);    // SCL cao (release)
}

/**
 * @brief Ghi dữ liệu LED vào HC595
 *
 * Chuyển đổi vu_value (0-80) thành 10 bytes để bật LED tương ứng
 */
void HC595_Write(uint8_t vu_value, uint8_t peak_value)
{
    // Giới hạn giá trị
    if(vu_value > 80) vu_value = 80;
    if(peak_value > 80) peak_value = 80;

    // Xóa buffer
    memset(led_buffer, 0, sizeof(led_buffer));

    // Bật LED từ 0 đến vu_value
    for(uint8_t i = 0; i < vu_value; i++) {
        uint8_t byte_idx = i / 8;      // Byte nào (0-9)
        uint8_t bit_idx = i % 8;       // Bit nào (0-7)
        led_buffer[byte_idx] |= (1 << bit_idx);
    }

    // Peak indicator (LED thứ peak_value sáng sáng hơn hoặc khác màu)
    if(peak_value > 0 && peak_value <= 80) {
        uint8_t byte_idx = (peak_value - 1) / 8;
        uint8_t bit_idx = (peak_value - 1) % 8;
        led_buffer[byte_idx] |= (1 << bit_idx);
    }

    // Gửi dữ liệu qua SPI
    GPIO_ResetBits(HC595_CS_PORT, HC595_CS_PIN);    // CS thấp (chọn)
    
    // Gửi 10 bytes (từ cuối về đầu để đảo ngược thứ tự)
    for(int i = 9; i >= 0; i--) {
        SPI_SendByte(led_buffer[i]);
    }
    
    GPIO_SetBits(HC595_CS_PORT, HC595_CS_PIN);      // CS cao (latch)
}

/**
 * @brief Xóa tất cả LED
 */
void HC595_Clear(void)
{
    memset(led_buffer, 0, sizeof(led_buffer));
    
    GPIO_ResetBits(HC595_CS_PORT, HC595_CS_PIN);
    for(int i = 0; i < 10; i++) {
        SPI_SendByte(0x00);
    }
    GPIO_SetBits(HC595_CS_PORT, HC595_CS_PIN);
}
