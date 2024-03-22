// #include "../../allwinner/wireless/btmanager4.0/src/include/bt_manager.h"
// #include "bt_manager.h"
#include "../../allwinner/btmanager/src/include/bt_manager.h"
#include <stdarg.h>
#include <stdio.h>
// #include <bt_manager.h>

#include "/home/bowen/OSHW/tina-d1-h/package/allwinner/wireless/btmanager4.0/src/include//bt_dev_list.h"

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

void bt_test_gap_display_pin_code_cb(char *device,char *pincode)
{
    printf("bt_test_gap_display_pin_code_cb\n");
}

void bt_test_gap_request_passkey_cb(void *handle,char *device)
{
    printf("bt_test_gap_request_passkey_cb\n");
}

void bt_test_gap_display_passkey_cb(char *device,unsigned int passkey,
		unsigned int entered)
{
    printf("bt_test_gap_display_passkey_cb\n");
}

void bt_test_gap_confirm_passkey_cb(void *handle,char *device,unsigned int passkey)
{
    printf("bt_test_gap_confirm_passkey_cb\n");
}

void bt_test_gap_authorize_cb(void *handle,char *device)
{
    printf("bt_test_gap_authorize_cb\n");
}

void bt_test_gap_authorize_service_cb(void *handle,char *device,char *uuid)
{
	printf("bt_test_gap_authorize_service_cb\n");
}
static btmg_callback_t *bt_callback = NULL;
int main() {
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
	bt_callback->btmg_gap_cb.gap_authorize_service = bt_test_gap_authorize_service_cb;

    bt_gatt_server_register_callback(bt_callback);
    bt_manager_init(bt_callback);
    bt_gatt_server_init();

    return 0;

}
