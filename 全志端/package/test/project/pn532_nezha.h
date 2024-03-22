#ifndef PN532_RPI
#define PN532_RPI

#include "pn532.h"

int PN532_Reset(void);
void PN532_Log(const char* log, ...);

void PN532_SPI_Init(PN532* dev);
int PN532_SPI_ReadData(uint8_t* data, uint16_t count);
int PN532_SPI_WriteData(uint8_t *data, uint16_t count);
bool PN532_SPI_WaitReady(uint32_t timeout);
int PN532_SPI_Wakeup(void);

void PN532_UART_Init(PN532* dev, int uartNum, unsigned long baudRate);
int PN532_UART_ReadData(uint8_t* data, uint16_t count);
int PN532_UART_WriteData(uint8_t *data, uint16_t count);
bool PN532_UART_WaitReady(uint32_t timeout);
int PN532_UART_Wakeup(void);

void PN532_I2C_Init(PN532* dev);
int PN532_I2C_ReadData(uint8_t* data, uint16_t count);
int PN532_I2C_WriteData(uint8_t *data, uint16_t count);
bool PN532_I2C_WaitReady(uint32_t timeout);
int PN532_I2C_Wakeup(void);

#endif  /* PN532_RPI */
