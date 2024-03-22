#include "pn532.h"
#include "pn532_nezha.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wifi_intf.h"
#include <time.h>
#include <sys/time.h>
// #define SUPPORT_I2C
#define SUPPORT_UART

int readBlock(PN532 * pn532,uint8_t * uid, int32_t uid_len, uint8_t * key, uint8_t blockNum, uint8_t * buf) {
    int pn532_error = PN532_MifareClassicAuthenticateBlock(pn532, uid, uid_len, blockNum, MIFARE_CMD_AUTH_A, key);
    if (pn532_error != PN532_ERROR_NONE) {
        printf("Authenticate error\n");;
    }
    pn532_error = PN532_MifareClassicReadBlock(pn532, buf, blockNum);
    if (pn532_error != PN532_ERROR_NONE) {
        printf("read block1 error\n");
    }
    return pn532_error;

}


static void wifi_state_handle(struct Manager *w, int event_label)
{
    printf("event_label 0x%x\n", event_label);

    switch(w->StaEvt.state)
    {
		 case CONNECTING:
		 {
			printf("Connecting to the network(%s)......\n",w->ssid);
			break;
		 }
		 case CONNECTED:
		 {
			printf("Connected to the AP(%s)\n",w->ssid);
			start_udhcpc();
			break;
		 }

		 case OBTAINING_IP:
		 {
			printf("Getting ip address(%s)......\n",w->ssid);
			break;
		 }

		 case NETWORK_CONNECTED:
		 {
			printf("Successful network connection(%s)\n",w->ssid);
			break;
		 }
		case DISCONNECTED:
		{
		    printf("Disconnected,the reason:%s\n",wmg_event_txt(w->StaEvt.event));
		    break;
		}
    }
}


int main(int argc, char const *argv[]) {
    PN532 pn532;
    
    uint8_t uid[MIFARE_UID_MAX_LENGTH];
    uint8_t key_a[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint32_t pn532_error = PN532_ERROR_NONE;
    int32_t uid_len = 0;
    printf("Start connecting...\n");
    printf("Reading NFC card...\n");
    #ifdef SUPPORT_UART
    PN532_UART_Init(&pn532, 0, 115200);
    #elif SUPPORT_I2C
    PN532_I2C_Init(&pn532);
    PN532_SamConfiguration(&pn532);
    #endif
    system("echo heartbeat > /sys/class/leds/sunxi_led0g/trigger");
    struct timespec tv_begin, tv_end;

    clock_gettime(CLOCK_MONOTONIC, &tv_begin);

    while (1)
    {
        // Check if a card is available to read
        uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 1000);
        if (uid_len == PN532_STATUS_ERROR) {
            printf(".");
        } else {
            printf("Found card with UID: ");
            for (uint8_t i = 0; i < uid_len; i++) {
                printf("%02x ", uid[i]);
            }
            printf("\n");
            break;
        }
    }

    uint8_t ssid[16] = {0};
    uint8_t pwd[16] = {0};
    const aw_wifi_interface_t *p_wifi_interface = NULL;
    int event_label = 0;
    int retry = 10;
    
    while(retry > 0) {
        printf("Reading blocks...\n");

        pn532_error = readBlock(&pn532, uid, uid_len, key_a, 1, ssid);
        printf("%d: ", 1);
        for (uint8_t i = 0; i < 16; i++) {
            printf("%02x ", ssid[i]);
        }
        printf("\n");

        pn532_error = readBlock(&pn532, uid, uid_len, key_a, 2, pwd);
        printf("%d: ", 2);
        for (uint8_t i = 0; i < 16; i++) {
            printf("%02x ", pwd[i]);
        }
        printf("\n");
        p_wifi_interface = aw_wifi_on(wifi_state_handle, event_label);
        if(p_wifi_interface == NULL){
            printf("wifi on failed\n");
        }

        while(aw_wifi_get_wifi_state() == CONNECTING) {
            sleep(1);
            printf("waiting connected......");
            break;
        }

        if(aw_wifi_get_wifi_state() == NETWORK_CONNECTED){
            printf("auto connected Successful  !!!!\n");
            printf("==================================\n");
            break;
        }

        p_wifi_interface->connect_ap(ssid, pwd, event_label);
        if(aw_wifi_get_wifi_state() == NETWORK_CONNECTED) {
            printf("Wifi connect ap : Success!\n");
            break;
        }
        else {
            printf("Wifi connect ap : Failure!\n");
            // return -1; //flag for tinatest
        }
        printf("==================================\n");
        retry --;
    }
    clock_gettime(CLOCK_MONOTONIC, &tv_end);

    double total_time = (double) (tv_end.tv_sec - tv_begin.tv_sec) +
                 (tv_end.tv_nsec - tv_begin.tv_nsec) / 1e9;

    printf("\n time cost: %.3f s\n", total_time);


    system("echo 0 > /sys/class/leds/sunxi_led0g/brightness");
    system("ping www.baidu.com");
    return 0;
}