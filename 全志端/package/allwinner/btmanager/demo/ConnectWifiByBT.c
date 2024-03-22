
#include <stdarg.h>
#include <stdio.h>
#include <bt_manager.h>

#include "bt_dev_list.h"
#include "bt_test.h"
#include <stdlib.h>
#include <string.h>
#include "wifi_intf.h"
#include <pthread.h>
// void btmg_print(int level, const char *fmt, ...)
// {
//     va_list aptr;
//     int ret;
//     char buffer[100];
//     va_start(aptr, fmt);
//     ret = vsprintf(buffer, fmt, aptr);
//     va_end(aptr);
//     printf(buffer);
// }
static char running = 1;

static void bt_test_manager_cb(int event_id)
{
	printf("bt test callback function enter, event_id: %d", event_id);
}

static void bt_test_adapter_power_state_cb(btmg_adapter_power_state_t state)
{
    printf("bt_test_adapter_power_state_cb\n");
}

static void bt_test_status_cb(btmg_state_t status)
{
    printf("bt_test_status_cb\n");
}

static void bt_test_discovery_status_cb(btmg_discovery_state_t status)
{
    printf("bt_test_discovery_status_cb\n");
}

static void bt_test_gap_connected_changed_cb(btmg_bt_device_t *device)
{
    printf("bt_test_gap_connected_changed_cb\n");
}

static void bt_test_dev_add_cb(btmg_bt_device_t *device)
{
    printf("bt_test_dev_add_cb\n");
}
static void bt_test_dev_remove_cb(btmg_bt_device_t *device)
{
    printf("bt_test_dev_remove_cb\n");
}

static void bt_test_update_rssi_cb(const char *address, int rssi)
{
    printf("bt_test_update_rssi_cb\n");
}

static void bt_test_bond_state_cb(btmg_bond_state_t state,const  char *bd_addr,const char *name)
{
    printf("bt_test_bond_state_cb\n");
}

static void bt_test_pair_ask(const char *prompt,char *buffer)
{
    printf("bt_test_pair_ask\n");
}

void bt_test_gap_request_pincode_cb(void *handle,char *device)
{
    printf("bt_test_gap_request_pincode_cb\n");
}

static void bt_test_gap_display_pin_code_cb(char *device,char *pincode)
{
    printf("bt_test_gap_display_pin_code_cb\n");
}

static void bt_test_gap_request_passkey_cb(void *handle,char *device)
{
    printf("bt_test_gap_request_passkey_cb\n");
}

static void bt_test_gap_display_passkey_cb(char *device,unsigned int passkey,
		unsigned int entered)
{
    printf("bt_test_gap_display_passkey_cb\n");
}

static void bt_test_gap_confirm_passkey_cb(void *handle,char *device,unsigned int passkey)
{
    printf("bt_test_gap_confirm_passkey_cb\n");
}

static void bt_test_gap_authorize_cb(void *handle,char *device)
{
    printf("bt_test_gap_authorize_cb\n");
}

static void bt_test_gap_authorize_service_cb2(void *handle,char *device,char *uuid)
{
	printf("bt_test_gap_authorize_service_cb2\n");
}
static void bt_test_gatt_connection_cb2(char *addr, gatt_connection_event_t event)
{
	if(event == BT_GATT_CONNECTION) {
		printf("gatt server connected to device: %s.\n", addr);
	} else if(event == BT_GATT_DISCONNECT) {
		printf("gatt server disconnected to device: %s.\n", addr);
	} else {
		printf("gatt server event unkown.\n");
	}

}
static int char_handle;
static int dev_name_char_handle;
static void bt_test_gatt_char_read_request_cb2(gatt_char_read_req_t *chr_read)
{
	char value[1];
	static unsigned char count = 0;
	char dev_name[] = "just-stop-it";
    printf("bt_test_gatt_char_read_request_cb2\n");
	printf("trans_id:%d,attr_handle:%d,offset:%d\n",chr_read->trans_id,
		chr_read->attr_handle,chr_read->offset);

	if(chr_read) {
		if (chr_read->attr_handle == dev_name_char_handle) {
			gatt_send_read_rsp_t data;
			data.trans_id = chr_read->trans_id;
			data.svc_handle = chr_read->attr_handle;
			data.status = 0x0b;
			data.auth_req = 0x00;
			value[0]= count;
			data.value = dev_name;
			data.value_len = strlen(dev_name);
			bt_manager_gatt_send_read_response(&data);
			return;
		}
		gatt_send_read_rsp_t data;
		data.trans_id = chr_read->trans_id;
		data.svc_handle = chr_read->attr_handle;
		data.status = 0x0b;
		data.auth_req = 0x00;
		value[0]= count;
		data.value = value;
		data.value_len = 1;
		bt_manager_gatt_send_read_response(&data);
		count ++;
	}
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
			running = 0;
			break;
		 }
		case DISCONNECTED:
		{
		    printf("Disconnected,the reason:%s\n",wmg_event_txt(w->StaEvt.event));
		    break;
		}
    }
	printf("wifi_state_handle finish\n");
}
static char ssid[16] = {0};
static char pwd[16] = {0};
static pthread_mutex_t mutex; // 互斥锁
static void bt_test_gatt_char_write_request_cb2(gatt_char_write_req_t *char_write)
{
    printf("bt_test_gatt_char_write_request_cb2\n");
	printf("enter_func %d\n", __LINE__);
	static unsigned char data_value[1] = {0};
	int ret = 0;

	if(char_write) {
		printf("attr_handle: %d,tran_id: %d\n",char_write->attr_handle,
			char_write->trans_id);
		printf("Value:");
		bt_manager_hex_dump(" ", 20, (unsigned char *)char_write->value, char_write->value_len);
        int j = 0, k = 0;

        char flag = 1;
        for (int i = 0; i < char_write->value_len; i++) {
            char c = char_write->value[i];
            if (c == '\n') {
                flag = 0;
                continue;
            }
            if (flag)
                ssid[j++] = char_write->value[i];
            else
                pwd[k++] = char_write->value[i];
        }
		pthread_mutex_lock(&mutex);
    	running = 0;
    	pthread_mutex_unlock(&mutex);
		running=0;
	}

	printf("char_write->need_rsp: %d\n", char_write->need_rsp);
	if (char_write->need_rsp) {
		gatt_write_rsp_t data;
		data.trans_id = char_write->trans_id;
		data.attr_handle = char_write->attr_handle;
		data.state = BT_GATT_SUCCESS;
		ret = bt_manager_gatt_send_write_response(&data);
		if (ret != 0)
			printf("send write response failed!\n");
		else
			printf("send write response success!\n");
	}
}


static btmg_callback_t *bt_callback = NULL;
int main() {
	system("echo heartbeat > /sys/class/leds/sunxi_led0b/trigger");
    bt_manager_preinit(&bt_callback);
    bt_manager_enable_profile(BTMG_GATT_SERVER_ENABLE);

	bt_callback->btmg_manager_cb.bt_mg_cb = bt_test_manager_cb;
	bt_callback->btmg_adapter_cb.adapter_power_state_cb = bt_test_adapter_power_state_cb;

	bt_callback->btmg_gap_cb.gap_status_cb = bt_test_status_cb;
	bt_callback->btmg_gap_cb.gap_disc_status_cb = bt_test_discovery_status_cb;
	bt_callback->btmg_gap_cb.gap_device_add_cb = bt_test_dev_add_cb;
	bt_callback->btmg_gap_cb.gap_device_remove_cb = bt_test_dev_remove_cb;
	bt_callback->btmg_gap_cb.gap_update_rssi_cb =	bt_test_update_rssi_cb;
	bt_callback->btmg_gap_cb.gap_bond_state_cb = bt_test_bond_state_cb;
	bt_callback->btmg_gap_cb.gap_connect_changed = bt_test_gap_connected_changed_cb;
	/* bt security callback setting.*/
	bt_callback->btmg_gap_cb.gap_request_pincode = bt_test_gap_request_pincode_cb;
	bt_callback->btmg_gap_cb.gap_display_pin_code = bt_test_gap_display_pin_code_cb;
	bt_callback->btmg_gap_cb.gap_request_passkey = bt_test_gap_request_passkey_cb;
	bt_callback->btmg_gap_cb.gap_display_passkey = bt_test_gap_display_passkey_cb;
	bt_callback->btmg_gap_cb.gap_confirm_passkey = bt_test_gap_confirm_passkey_cb;
	bt_callback->btmg_gap_cb.gap_authorize  = bt_test_gap_authorize_cb;
	bt_callback->btmg_gap_cb.gap_authorize_service = bt_test_gap_authorize_service_cb2;
	bt_gatt_server_register_callback(bt_callback);
	bt_callback->btmg_gatt_server_cb.gatt_char_read_req_cb = bt_test_gatt_char_read_request_cb2;
	bt_callback->btmg_gatt_server_cb.gatt_char_write_req_cb = bt_test_gatt_char_write_request_cb2;
    // bt_gatt_server_register_callback(bt_callback);
    bt_callback->btmg_gatt_server_cb.gatt_connection_event_cb = bt_test_gatt_connection_cb2;
    bt_manager_init(bt_callback);
	bt_manager_enable(true);
    bt_gatt_server_init();

    while (1) {
		pthread_mutex_lock(&mutex); // 加锁
		int variable_value = running;
        pthread_mutex_unlock(&mutex); // 解锁
		if (!variable_value)
			break;
    }
RECEIVE:
	const aw_wifi_interface_t *p_wifi_interface = NULL;
	int event_label = 0;
	int retry = 1;
		
	while(retry > 0) {
		p_wifi_interface = aw_wifi_on(wifi_state_handle, event_label);
		if(p_wifi_interface == NULL){
			printf("wifi on failed\n");
			break;
		}

		while(aw_wifi_get_wifi_state() == CONNECTING) {
			// sleep(1);
			printf("waiting connected......");
		}

		if(aw_wifi_get_wifi_state() == NETWORK_CONNECTED){
			printf("auto connected Successful  !!!!\n");
			printf("==================================\n");
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
	system("echo 0 > /sys/class/leds/sunxi_led0b/brightness");
	system("ping www.baidu.com");
    return 0;

}
