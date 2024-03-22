#include "pn532.h"
#include "pn532_nezha.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]) {
    PN532 pn532;
    
    uint8_t uid[MIFARE_UID_MAX_LENGTH];
    uint8_t key_a[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint32_t pn532_error = PN532_ERROR_NONE;
    int32_t uid_len = 0;
    printf("init begin\n");
    PN532_UART_Init(&pn532, 0, 115200);
    // PN532_I2C_Init(&pn532);
    // PN532_SamConfiguration(&pn532);
    printf("init finish\n");
    while (1)
    {
        // Check if a card is available to read
        printf("read begin\n");
        uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 1000);
        if (uid_len == PN532_STATUS_ERROR) {
            printf(".");
        } else {
            printf("Found card with UID: ");
            for (uint8_t i = 0; i < uid_len; i++) {
                printf("%02x ", uid[i]);
            }
            printf("\r\n");
            break;
        }
    }

    uint8_t ssid[16];
    uint8_t pwd[16];
    printf("Reading blocks...\r\n");

    pn532_error = PN532_MifareClassicAuthenticateBlock(&pn532, uid, uid_len, 1, MIFARE_CMD_AUTH_A, key_a);
    if (pn532_error != PN532_ERROR_NONE) {
        printf("Authenticate error\r\n");;
    }
    pn532_error = PN532_MifareClassicReadBlock(&pn532, ssid, 1);
    if (pn532_error != PN532_ERROR_NONE) {
        printf("read block1 error\r\n");
    }
    printf("%d: ", 1);
    for (uint8_t i = 0; i < 16; i++) {
        printf("%02x ", ssid[i]);
    }
    printf("\r\n");

    pn532_error = PN532_MifareClassicAuthenticateBlock(&pn532, uid, uid_len, 2, MIFARE_CMD_AUTH_A, key_a);
    if (pn532_error != PN532_ERROR_NONE) {
        printf("Authenticate error\r\n");;
    }
    pn532_error = PN532_MifareClassicReadBlock(&pn532, pwd, 2);
    if (pn532_error != PN532_ERROR_NONE) {
        printf("read block2 error\r\n");
    }
    printf("%d: ", 2);
    for (uint8_t i = 0; i < 16; i++) {
        printf("%02x ", pwd[i]);
    }
    printf("\r\n");


    return 0;
}