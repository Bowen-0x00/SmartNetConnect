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

#include "bt_dev_list.h"
#include "bt_log.h"
#include "bt_test.h"


#define TEST_SERVICE_UUID    "1112"
#define TEST_CHAR_UUID1      "2223"
#define TEST_CHAR_UUID2      "3334"
#define DEV_NAME_CHAR_UUID2  "5555"
#define CCCD_UUID            "2902"

#define TEST_DESC_UUID      "00006666-0000-1000-8000-00805F9B34FB"

static uint16_t service_handle;
void bt_test_gatt_connection_cb(char *addr, gatt_connection_event_t event)
{
	if(event == BT_GATT_CONNECTION) {
		printf("gatt server connected to device: %s.\n", addr);
	} else if(event == BT_GATT_DISCONNECT) {
		printf("gatt server disconnected to device: %s.\n", addr);
	} else {
		printf("gatt server event unkown.\n");
	}

}

void bt_test_gatt_add_service_cb(gatt_add_svc_msg_t *msg)
{
	if(msg != NULL) {
		service_handle = msg->svc_handle;
		printf("add service handle is %d of number_hanle: %d\n",service_handle, msg->num_handle);
	}

}

static int char_handle;
static int dev_name_char_handle;
void bt_test_gatt_add_char_cb(gatt_add_char_msg_t *msg)
{
	if(msg != NULL) {
		printf("add char,uuid: %s,chr handle is %d\n",msg->uuid,msg->char_handle);
		if (strcmp(msg->uuid, DEV_NAME_CHAR_UUID2) == 0)
			dev_name_char_handle = msg->char_handle;
		else
			char_handle = msg->char_handle;
	}
}

void bt_test_gatt_add_desc_cb(gatt_add_desc_msg_t *msg)
{
	if(msg != NULL) {
		printf("desc handle is %d\n", msg->desc_handle);
	}

}

void bt_test_gatt_char_read_request_cb(gatt_char_read_req_t *chr_read)
{
	char value[1];
	static unsigned char count = 0;
	char dev_name[] = "aw_ble_test_1149";

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
void bt_test_gatt_send_indication_cb(gatt_send_indication_t *send_ind)
{
	printf("enter_func %d\n", __LINE__);
}

void bt_test_gatt_char_write_request_cb(gatt_char_write_req_t *char_write)
{
	printf("enter_func %d\n", __LINE__);
	static unsigned char data_value[1] = {0};
	int ret = 0;

	if(char_write) {
		printf("attr_handle: %d,tran_id: %d\n",char_write->attr_handle,
			char_write->trans_id);
		printf("Value:");
		bt_manager_hex_dump(" ", 20, (unsigned char *)char_write->value, char_write->value_len);
	}

/*
	gatt_notify_rsp_t data;
	data.attr_handle = char_write->attr_handle;
	data.value = data_value;
	data.value_len = 1;

	data_value[0] ++ ;
	printf("send data value:%d\n",data_value[0]);
	bt_manager_gatt_send_notification(&data);
*/

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

void bt_test_gatt_char_notify_request_cb(gatt_notify_req_t *char_notify)
{
	printf("enter\n");
	printf("enter_func %d\n", __LINE__);

}

void bt_test_gatt_desc_read_requset_cb(gatt_desc_read_req_t *desc_read)
{
	printf("enter\n");
	char value[1];
	static unsigned char count = 0;
	printf("enter\n");

	printf("trans_id:%d,attr_handle:%d,offset:%d\n",desc_read->trans_id,
		desc_read->attr_handle,desc_read->offset);

	if(desc_read) {
		gatt_send_read_rsp_t data;
		data.trans_id = desc_read->trans_id;
		data.svc_handle = desc_read->attr_handle;
		data.status = 0x0b;
		data.auth_req = 0x00;
		value[0]= count;
		data.value = value;
		data.value_len = 1;
		bt_manager_gatt_send_read_response(&data);
		count ++;
	}
}

void bt_test_gatt_desc_write_request_cb(gatt_desc_write_req_t *desc_write)
{
	int ret = 0;

	printf("enter\n");
	printf("desc_write->need_rsp: %d\n", desc_write->need_rsp);
	printf("desc_write->attr_handle: %d\n", desc_write->attr_handle);

	if (desc_write->need_rsp) {
		gatt_write_rsp_t data;
		data.trans_id = desc_write->trans_id;
		data.attr_handle = desc_write->attr_handle;
		data.state = BT_GATT_SUCCESS;
	    ret = bt_manager_gatt_send_write_response(&data);
	    if (ret != 0)
			printf("send write response failed!\n");
		else
			printf("send write response success!\n");
	}
}

static void set_adv_param(void)
{
	btmg_le_advertising_parameters_t adv_params;

	adv_params.min_interval = 0x0020;
	adv_params.max_interval = 0x01E0; /*range from 0x0020 to 0x4000*/
	adv_params.own_addr_type = BTMG_LE_RANDOM_ADDRESS;
	adv_params.adv_type =  BTMG_LE_ADV_IND; /*ADV_IND*/
	adv_params.chan_map = BTMG_LE_ADV_CHANNEL_ALL; /*0x07, *bit0:channel 37, bit1: channel 38, bit2: channel39*/
	adv_params.filter = BTMG_LE_PROCESS_ALL_REQ;
	bt_manager_set_adv_param(&adv_params);

}


static void le_set_adv_data()
{
	int dd;
	uint8_t manuf_len;
	uint16_t ogf, ocf;
	int index;
	char advdata[32] = { 0 };
	char uuid_buf[5] = {0};
	const char *ble_name="aw-ble-test-007";
//#define AD_LEN_MANUF	16
#define AD_LEN_FLAG		3
//#define AD_LEN_APPEARANCE	4
//#define AD_LEN_LIST_OF_UUID	4
#define AD_LEN_LOCAL_NAME	10

	index = 1;
//	manuf_len = 1 + 14;
//	advdata[index] = manuf_len;	/* manuf len */
//	advdata[index + 1] = 0xff;		/* ad type */
//	memcpy(advdata + 3, manufacturer_data, MANUFACTURER_DATA_LEN + WIFI_LEN);

//	index += AD_LEN_MANUF;
	advdata[index] = 0x02;			/* flag len */
	advdata[index + 1] = 0x01;		/* type for flag */
	advdata[index + 2] = 0x1A;  //0x05
	index += AD_LEN_FLAG;

//	advdata[index] = 3;				/* appearance len */
//	advdata[index + 1] = 0x19;		/* type for appearance */
//	advdata[index + 2] = 0x01;
//	advdata[index + 3] = 0x00;

//	index += AD_LEN_APPEARANCE;
//	advdata[index] = 0x03;			/* uuid len */
//	advdata[index + 1] = 0x03;		/* type for complete list of 16-bit uuid */
//	advdata[index + 2] = 0x20;
//	advdata[index + 3] = 0x9e;


//	advdata[index] = 2+1;			/* uuid len */
//	advdata[index + 1] = 0x03;		/* type for  complete list of 16-bit uuid*/
//	sscanf(uuid_buf+2, "%2x", &advdata[index + 2]);		/* type for  complete list of 16-bit uuid*/
//	uuid_buf[2] = '\0';
//	sscanf(uuid, "%2x", &advdata[index + 3]);		/* type for  complete list of 16-bit uuid*/
//	index += AD_LEN_LIST_OF_UUID;

//	advdata[index] = 0x0A;			/* uuid len */
	advdata[index] = strlen(ble_name)+1;			/* name len */
	advdata[index + 1] = 0x09;		/* type for local name */
	index+=2;
	int i, name_len;
	name_len = strlen(ble_name);
	for(i=0;i<=name_len;i++) {
		advdata[index + i] = ble_name[i];
	}

	// total length
//	advdata[0] = AD_LEN_MANUF + AD_LEN_FLAG + AD_LEN_APPEARANCE	+ AD_LEN_LIST_OF_UUID + AD_LEN_LOCAL_NAME;
	//advdata[0] = AD_LEN_FLAG + AD_LEN_LIST_OF_UUID + (name_len + 2);
	advdata[0] = AD_LEN_FLAG + (name_len + 2);

	btmg_adv_rsp_data_t adv;
	adv.data_len = advdata[0];
	memcpy(adv.data,&advdata[1],31);

	bt_manager_le_set_adv_data(&adv);

	bt_manager_le_enable_adv(true);
}

void bt_gatt_server_register_callback(btmg_callback_t *cb)
{
	cb->btmg_gatt_server_cb.gatt_add_svc_cb = bt_test_gatt_add_service_cb;
	cb->btmg_gatt_server_cb.gatt_add_char_cb = bt_test_gatt_add_char_cb;
	cb->btmg_gatt_server_cb.gatt_add_desc_cb = bt_test_gatt_add_desc_cb;
	cb->btmg_gatt_server_cb.gatt_connection_event_cb = bt_test_gatt_connection_cb;
	cb->btmg_gatt_server_cb.gatt_char_read_req_cb = bt_test_gatt_char_read_request_cb;
	cb->btmg_gatt_server_cb.gatt_char_write_req_cb = bt_test_gatt_char_write_request_cb;
	cb->btmg_gatt_server_cb.gatt_char_notify_req_cb = bt_test_gatt_char_notify_request_cb;
	cb->btmg_gatt_server_cb.gatt_desc_read_req_cb = bt_test_gatt_desc_read_requset_cb;
	cb->btmg_gatt_server_cb.gatt_desc_write_req_cb = bt_test_gatt_desc_write_request_cb;
	cb->btmg_gatt_server_cb.gatt_send_indication_cb = bt_test_gatt_send_indication_cb;
}

int bt_gatt_server_init()
{
	gatt_add_svc_t svc;
	gatt_add_char_t chr1;
	gatt_add_char_t chr2;
	gatt_add_char_t chr3;
	gatt_add_desc_t desc;
	gatt_star_svc_t start_svc;

	bt_manager_gatt_server_init();

	printf("add service,uuid:%s\n",TEST_SERVICE_UUID);
	svc.number = 10;
	svc.uuid = TEST_SERVICE_UUID;
	svc.primary = true;
	bt_manager_gatt_create_service(&svc);

	chr1.permissions = BT_GATT_PERM_READ | BT_GATT_PERM_WRITE;
	chr1.properties = BT_GATT_CHAR_PROPERTY_READ | BT_GATT_CHAR_PROPERTY_WRITE | BT_GATT_CHAR_PROPERTY_INDICATE;
	chr1.svc_handle = service_handle;
	chr1.uuid = TEST_CHAR_UUID1;
	bt_manager_gatt_add_characteristic(&chr1);

	desc.permissions = BT_GATT_PERM_READ | BT_GATT_PERM_WRITE;
	desc.uuid = CCCD_UUID;
	desc.svc_handle = service_handle;
	bt_manager_gatt_add_descriptor(&desc);

	chr2.permissions = BT_GATT_PERM_READ | BT_GATT_PERM_WRITE;
	chr2.properties = BT_GATT_CHAR_PROPERTY_READ | BT_GATT_CHAR_PROPERTY_WRITE |
		BT_GATT_CHAR_PROPERTY_NOTIFY;
	chr2.svc_handle = service_handle;
	chr2.uuid = TEST_CHAR_UUID2;
	bt_manager_gatt_add_characteristic(&chr2);

	chr3.permissions = BT_GATT_PERM_READ;
	chr3.properties = BT_GATT_CHAR_PROPERTY_READ;
	chr3.svc_handle = service_handle;
	chr3.uuid = DEV_NAME_CHAR_UUID2;
	bt_manager_gatt_add_characteristic(&chr3);

	bt_manager_gatt_start_service(&start_svc);

	set_adv_param();
	bt_manager_le_set_random_address();
	le_set_adv_data();

	return 0;
}

int bt_gatt_server_deinit()
{
	bt_manager_gatt_server_deinit();

	return 0;
}

int bt_test_send_indication(int attr_handle)
{
	char test_ind[] = "hello!!!";

	gatt_indication_rsp_t indication_data;

	indication_data.attr_handle = attr_handle;
	indication_data.value = test_ind;
	indication_data.value_len = strlen(test_ind);


	bt_manager_gatt_send_indication(&indication_data);

	return 0;
}

static void cmd_gatts_init(int argc,char *args[])
{
	bt_gatt_server_init();
	return ;
}

static void cmd_gatts_deinit(int argc,char *args[])
{
	bt_gatt_server_deinit();
	return ;
}

static void cmd_ble_adv(int argc, char *args[])
{
	int value = 0;

	if(argc != 1) {
		BTMG_ERROR("Unexpected argc: %d, see help", argc);
		return;
	}

	value = atoi(args[0]);
	BTMG_INFO("Set ble adv enable :%d", value);
	if(value)
		bt_manager_le_enable_adv(true);
	else
		bt_manager_le_enable_adv(false);
}


static void cmd_ble_ind(int argc, char *args[])
{
	int value = 0;

	if(argc != 1) {
		BTMG_ERROR("Unexpected argc: %d, see help", argc);
		return;
	}

	value = atoi(args[0]);

	bt_test_send_indication(value);
}

static void cmd_ble_disconnect(int argc,char *args[])
{
	bt_manager_le_disconnect();
}

cmd_tbl_t bt_gatts_cmd_table[] = {
	{"gatts_init",        NULL , cmd_gatts_init,               "gatts_init: gatt server init"},
	{"gatts_deinit",      NULL , cmd_gatts_deinit,             "gatts_deinit:gatt server deinit"},
	{"ble_adv",           NULL , cmd_ble_adv,                  "ble_adv [0/1]: Disable/Enable ble advertising"},
	{"ble_ind",           NULL , cmd_ble_ind,                  "ble_ind [char_handle]"},
	{"ble_dis",           NULL , cmd_ble_disconnect,           "ble_dis: disconnect le"},
	{ NULL, NULL, NULL, NULL},
};


