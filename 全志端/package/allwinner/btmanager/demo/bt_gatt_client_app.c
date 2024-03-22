#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/signalfd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <getopt.h>
#include <bt_manager.h>
#include "bt_log.h"
#include "bt_dev_list.h"
#include "bt_test.h"


void *gattc_cn_handle = NULL;

#define CMD_NOTIFY_ID_MAX	20
static int notify_id_save[CMD_NOTIFY_ID_MAX + 1];

static void bt_test_gattc_scan_report_cb(btmg_le_scan_report_t *data)
{
	int j=0;
	int type;

	char name[31] = {0};

	for(;;) {
		type = data->report.data[j+1];
		if(type == 0x09){ //complete local name.
			memcpy(name,&data->report.data[j+2],data->report.data[j] -1);
			name[data->report.data[j] -1] = '\0';
			BTMG_INFO("name:%s",name);
		}
		j = j + data->report.data[j] + 1;
		if(j >= data->report.data_len)
			break;
	}
	bt_manager_hex_dump("peer_addr",6,data->peer_addr,6);
	//BTMG_INFO("addr type: %d , adv type:%d, rssi:%d",data->addr_type,
	//		data->adv_type,data->rssi);
	//bt_manager_hex_dump("adv data",20,data->report.data,data->report.data_len);
}

static void bt_test_gattc_update_rssi_cb(const char *address, int rssi)
{
	dev_node_t *dev_node = NULL;

	dev_node = btmg_dev_list_find_device(discovered_devices, address);
	if (dev_node != NULL) {
		BTMG_INFO("address:%s,rssi:%d", address, rssi);
		return;
	}

}


void bt_test_gattc_notify_cb(gattc_notify_cb_para_t *data)
{
	int i;

	printf("\n\tHandle Value Not/Ind: 0x%04x - ", data->value_handle);

	if (data->length == 0) {
		printf("(0 bytes)\n");
		return;
	}

	printf("(%u bytes): ", data->length);

	for (i = 0; i < data->length; i++)
		printf("%02x ", data->value[i]);

	printf("\n");

}

void bt_test_gattc_write_long_cb(gattc_write_long_cb_para_t *data)
{
	if (data->success) {
		printf("Write successful\n");
	} else if(data->reliable_error) {
		printf("Reliable write not verified\n");
	} else {
		printf("\nWrite failed: %s (0x%02x)\n",
				bt_manager_gatt_client_ecode_to_string(data->att_ecode),
					data->att_ecode);
	}
}

void bt_test_gattc_write_cb(gattc_write_cb_para_t *data)
{
	if (data->success) {
		printf("\nWrite successful\n");
	} else {
		printf("\nWrite failed: %s (0x%02x)\n",
				bt_manager_gatt_client_ecode_to_string(data->att_ecode),
					data->att_ecode);
	}
}

void bt_test_gattc_read_cb(gattc_read_cb_para_t *data)
{
	int i;

	if (!data->success) {
		printf("\nRead request failed: %s (0x%02x)\n",
				bt_manager_gatt_client_ecode_to_string(data->att_ecode),
					data->att_ecode);
		return;
	}

	printf("\nRead value");

	if (data->length == 0) {
		printf(": 0 bytes\n");
		return;
	}

	printf(" (%u bytes): ", data->length);

	for (i = 0; i < data->length; i++)
		printf("%02x ", data->value[i]);

	printf("\n");

}

void bt_test_gattc_conn_cb(gattc_conn_cb_para_t *data)
{
	if (!data->success) {
		printf("GATT discovery procedures failed - error code: 0x%02x\n",
								data->att_ecode);
		return;
	}
	gattc_cn_handle = data->cn_handle;
	printf("GATT discovery procedures complete\n");

}

void bt_test_gattc_disconn_cb(gattc_disconn_cb_para_t *data)
{
	printf("Device disconnected:%s\n",strerror(data->err));
}

void bt_test_gattc_service_changed_cb(gattc_service_changed_cb_para_t *data)
{
	printf("\nService Changed handled - start: 0x%04x end: 0x%04x\n",
			data->start_handle, data->end_handle);
}

static void print_uuid(const btmg_uuid_t *uuid)
{
	char uuid_str[37];
	btmg_uuid_t uuid128;

	bt_manager_uuid_to_uuid128(uuid, &uuid128);
	bt_manager_uuid_to_string(&uuid128, uuid_str, sizeof(uuid_str));

	printf("%s\n", uuid_str);
}


void bt_test_gattc_dis_service_cb(gattc_dis_service_cb_para_t *data)
{

	printf("service - start: 0x%04x, "
				"end: 0x%04x, type: %s, uuid: ",
				data->start_handle, data->end_handle,
				data->primary ? "primary" : "secondary");
	print_uuid(&data->uuid);
	bt_manager_gatt_client_discover_service_all_char(data->attr);
}

void bt_test_gattc_dis_char_cb(gattc_dis_char_cb_para_t *data)
{

	printf("\t  charac- start: 0x%04x, value: 0x%04x, "
				"props: 0x%02x, ext_props: 0x%04x, uuid: ",
				data->start_handle, data->value_handle, data->properties,
				data->ext_prop);
	print_uuid(&data->uuid);
	bt_manager_gatt_client_discover_char_all_descriptor(data->attr);
}

void bt_test_gattc_dis_desc_cb(gattc_dis_desc_cb_para_t *data)
{

	printf("\t\t  descr - handle: 0x%04x, uuid: ",data->handle);
	print_uuid(&data->uuid);
}

void bt_gatt_client_register_callback(btmg_callback_t *cb)
{
	cb->btmg_gatt_client_cb.gattc_conn_cb = bt_test_gattc_conn_cb;
	cb->btmg_gatt_client_cb.gattc_disconn_cb = bt_test_gattc_disconn_cb;
	cb->btmg_gatt_client_cb.gattc_read_cb = bt_test_gattc_read_cb;
	cb->btmg_gatt_client_cb.gattc_write_cb = bt_test_gattc_write_cb;
	cb->btmg_gatt_client_cb.gattc_write_long_cb = bt_test_gattc_write_long_cb;
	cb->btmg_gatt_client_cb.gattc_service_changed_cb = bt_test_gattc_service_changed_cb;
	cb->btmg_gatt_client_cb.gattc_notify_cb = bt_test_gattc_notify_cb;
	cb->btmg_gatt_client_cb.gattc_dis_service_cb = bt_test_gattc_dis_service_cb;
	cb->btmg_gatt_client_cb.gattc_dis_char_cb = bt_test_gattc_dis_char_cb;
	cb->btmg_gatt_client_cb.gattc_dis_desc_cb = bt_test_gattc_dis_desc_cb;

	cb->btmg_gap_cb.gap_le_scan_report_cb = bt_test_gattc_scan_report_cb;
	cb->btmg_gap_cb.gap_update_rssi_cb = bt_test_gattc_update_rssi_cb;
}

int bt_gatt_client_init()
{
	bt_manager_gatt_client_init();
	return 0;
}

int bt_gatt_client_deinit()
{
	bt_manager_gatt_client_deinit();
	return 0;
}


static void cmd_gatt_client_connect(int argc,char *args[])
{
	if(argc != 1) {
		goto end;
	}

	if(strlen(args[0]) < 17) {
		goto end;
	}

	bt_manager_gatt_client_connect(0,(uint8_t *)args[0],BTMG_BDADDR_LE_RANDOM,
			517,BTMG_SECURITY_LOW);
	return;
end:
	BTMG_ERROR("Unexpected argc: %d, see help", argc);
}

static void cmd_gatt_client_disconnect(int argc,char *args[])
{
	if(argc != 1) {
		goto end;
	}

	if(strlen(args[0]) < 17) {
		goto end;
	}

	bt_manager_gatt_client_disconnect(0, (uint8_t *)args[0]);
	return;
end:
	BTMG_ERROR("Unexpected argc: %d, see help", argc);
}


static void cmd_gatt_client_write_request(int argc,char *args[])
{
	static uint8_t value[1] = {0};
	uint16_t handle;
	char *endptr = NULL;
	if(argc != 1) {
		goto end;
	}

	handle = strtol(args[0], &endptr, 0);
	bt_manager_gatt_client_write_request(gattc_cn_handle,handle,value,1);
	value[0]++;

	return;
end:
	BTMG_ERROR("Unexpected argc: %d, see help", argc);
}


static void cmd_gatt_client_write_command(int argc,char *args[])
{
	uint16_t handle;
	char *endptr = NULL;
	static uint8_t value[1] = {0};
	if(argc != 1) {
		goto end;
	}

	handle = strtol(args[0], &endptr, 0);

	bt_manager_gatt_client_write_command(gattc_cn_handle,handle,
			false,value,1);
	value[0]++;
	return;
end:
	BTMG_ERROR("Unexpected argc: %d, see help", argc);
}

static void cmd_gatt_client_read_request(int argc,char *args[])
{
	uint16_t handle;
	char *endptr = NULL;
	if(argc != 1) {
		goto end;
	}

	handle = strtol(args[0], &endptr, 0);

	bt_manager_gatt_client_read_request(gattc_cn_handle,handle);
	return;
end:
	BTMG_ERROR("Unexpected argc: %d, see help", argc);
}

static void cmd_gatt_client_dis_all_pri_svcs(int argc,char *args[])
{

	bt_manager_gatt_client_discover_all_primary_services(gattc_cn_handle,
			0x0001,0xffff);
	return;
end:
	BTMG_ERROR("Unexpected argc: %d, see help", argc);
}

static void cmd_gatt_client_dis_pri_svcs_by_uuid(int argc,char *args[])
{
	if(argc != 1) {
		goto end;
	}

	if(strlen(args[0]) < 6) {
		goto end;
	}
	bt_manager_gatt_client_discover_primary_services_by_uuid(gattc_cn_handle,
			args[0],0x0001,0xffff);
	return;
end:
	BTMG_ERROR("Unexpected argc: %d, see help", argc);

}


static void cmd_ble_discovery(int argc,char *args[])
{
	int value = 0;
#if 0
	int ret = -1;
	btmg_scan_filter_t scan_filter = {0};

	scan_filter.type = BTMG_SCAN_LE;
	scan_filter.rssi = -90;

	if(argc != 1) {
		BTMG_ERROR("Unexpected argc: %d, see help", argc);
		return;
	}

	value = atoi(args[0]);
	if(value) {
		bt_manager_discovery_filter(&scan_filter);
		bt_manager_start_discovery();
	}
	else
		bt_manager_cancel_discovery();
#else
	if(argc != 1) {
		BTMG_ERROR("Unexpected argc: %d, see help", argc);
		return;
	}

	value = atoi(args[0]);
	if(value) {
		btmg_le_scan_param_t scan_param;
		btmg_le_scan_type_t scan_type;
		scan_param.scan_interval = 0x0010;
		scan_param.scan_window = 0x0010;
		scan_type = LE_SCAN_TYPE_ACTIVE;
		bt_manager_le_scan_start(0,scan_type,&scan_param,NULL);
	}else {
		bt_manager_le_scan_stop(0,NULL);
	}
#endif
}

static void cmd_gatt_client_get_mtu(int argc,char *args[])
{
	int mtu;
	mtu = bt_manager_gatt_client_get_mtu(gattc_cn_handle);
	if(mtu > 0) {
		printf("[MTU]:%d\n",mtu);
	}else {
		printf("get mtu failed.\n");
	}
	return;
}

static void cmd_gatt_client_register_notify(int argc,char *args[])
{
	uint16_t handle;
	int notify_id;
	int i;
	char *endptr = NULL;
	if(argc != 1) {
		goto end;
	}
	handle = strtol(args[0], &endptr, 0);
	notify_id = bt_manager_gatt_client_register_notify(gattc_cn_handle, handle);
	if (notify_id == 0){
		printf("register err\n");
	} else {
		for (i = 0; i < CMD_NOTIFY_ID_MAX && notify_id_save[i]; i++) {
			// do nothing
		}
		notify_id_save[i] = notify_id;
		printf("register with id = %d\n", notify_id);
	}
	return;
end:
	BTMG_ERROR("Unexpected argc: %d, see help", argc);
	return;
}

static void cmd_gatt_client_unregister_notify(int argc,char *args[])
{
	int i;
	for (i = 0; i < CMD_NOTIFY_ID_MAX && notify_id_save[i]; i++) {
		bt_manager_gatt_client_unregister_notify(gattc_cn_handle, notify_id_save[i]);
		notify_id_save[i] = 0;
	}
	return;
}

static void cmd_ble_set_scan_parameters(int argc,char *args[])
{
	btmg_le_scan_param_t scan_param;
	btmg_le_scan_type_t scan_type;
	uint8_t	own_type, filter_policy;

	scan_param.scan_interval = 0x0010;
	scan_param.scan_window = 0x0005;
	scan_type = LE_SCAN_TYPE_PASSIVE;
	own_type = BTMG_BDADDR_LE_RANDOM;
	filter_policy = 0x00;

	bt_manager_le_set_scan_parameters(0, scan_type, &scan_param, own_type, filter_policy);

	return;
}

static void cmd_ble_update_conn_params(int argc,char *args[])
{
	btmg_le_conn_param_t conn_params;

	conn_params.min_conn_interval = 0x0011;
	conn_params.max_conn_interval = 0x0055;
	conn_params.slave_latency = 0x0009;
	conn_params.conn_sup_timeout = 0x0180;

	bt_manager_le_update_conn_params(0, &conn_params);

	return;
}

static void cmd_gattc_init(int argc,char *args[])
{
	bt_manager_gatt_client_init();
	return ;
}

static void cmd_gattc_deint(int argc,char *args[])
{
	bt_manager_gatt_client_deinit();
	return ;
}

cmd_tbl_t bt_gattc_cmd_table[] = {
	{"gattc_init",        NULL , cmd_gattc_init,         "gattc_init: gatt client init"},
	{"gattc_deinit",      NULL , cmd_gattc_deint,        "gattc_deinit:gatt client deinit"},
	{"gattc_connect",           NULL , cmd_gatt_client_connect,         "connect [mac]:gatt client method to connect"},
	{"gattc_disconnect",        NULL , cmd_gatt_client_disconnect,      "disconnect [mac]:gatt client method to connect"},
	{"write-request",     NULL , cmd_gatt_client_write_request,   "write-request [handle]:gatt write request"},
	{"write-command",     NULL , cmd_gatt_client_write_command,   "write-command [handle]:gatt write command without response"},
	{"read-request",      NULL , cmd_gatt_client_read_request,    "read-request  [handle]:gatt read request"},
	{"ble_scan",          NULL , cmd_ble_discovery,               "ble_scan [0/1]: scan for devices"},
	{"dis_pri_svcs",      NULL , cmd_gatt_client_dis_all_pri_svcs,"services:discovery all primary services"},
	{"dis_svcs_uuid",     NULL , cmd_gatt_client_dis_pri_svcs_by_uuid,"dis_svcs_uuid [uuid]: discovery services by uuid"},
	{"get_mtu",           NULL , cmd_gatt_client_get_mtu,         "get_mtu:get MTU"},
	{"register-notify",   NULL , cmd_gatt_client_register_notify,   "register-notify [handle]:gatt register notify callback"},
	{"unregister-notify", NULL , cmd_gatt_client_unregister_notify, "unregister-notify:gatt unregister all notify callback"},
	{"set_scan_para",     NULL , cmd_ble_set_scan_parameters,     "set scan parameters"},
	{"update_conn_para",  NULL , cmd_ble_update_conn_params,      "update conn params"},
	{ NULL, NULL, NULL, NULL},
};


