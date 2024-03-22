#ifndef __BT_TEST_H__
#define __BT_TEST_H__

#include "bt_dev_list.h"

typedef struct {
	const char *cmd;
	const char *arg;
	void (*func) (int argc, char *args[]);
	const char *desc;
} cmd_tbl_t;

extern dev_list_t *bonded_devices;
extern dev_list_t *discovered_devices;
extern btmg_profile_info_t *bt_pro_info;

extern cmd_tbl_t bt_cmd_table[];
extern cmd_tbl_t bt_gatts_cmd_table[];
extern cmd_tbl_t bt_gattc_cmd_table[];


/*gatt client*/
void bt_gatt_client_register_callback(btmg_callback_t *cb);
int bt_gatt_client_init();
int bt_gatt_client_deinit();

/*gatt server*/
void bt_gatt_server_register_callback(btmg_callback_t *cb);
int bt_gatt_server_init();
int bt_gatt_server_deinit();

#endif