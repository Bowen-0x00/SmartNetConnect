#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/signalfd.h>
#include <ell/ell.h>
#include "cmd.h"

#include "cfgmod.h"
#include "util.h"
#define APP_IDX_DEV	0x7fff
#define VENDOR_ID_MASK		0xffff0000
int32_t __main_terminated;

char uuid[16] ={0xd8,0x01,0x51,0xcf,0x01,0x00,0x00,0x07,0x01,0xf5,0x07,0xda,0x78,0x00,0x00,0x00};
char str_uuid[33];


static  uint8_t local_app_key[16] = {0x66, 0x66, 0x66 ,0x66 ,0x66 ,0x66 ,0x66 ,0x66 ,0x66 ,0x66 ,0x66 ,0x66 ,0x66 ,0x66 ,0x66 ,0x66};
static  uint8_t local_new_app_key[16] = {0x88, 0x96, 0x47, 0x71, 0x73, 0x4f, 0xbd, 0x76,0xe3, 0xb4, 0x05, 0x19, 0xd1, 0xd9, 0x4a, 0x48};


#define RUN_XIAOMI_MESH_APP 0
#define RUN_PTS_MESH_APP 0

#if (RUN_PTS_MESH_APP == 1)
extern void pts_app_run();
extern void pts_app_reset();
extern void pts_app_onpowerup_reset();
extern void pts_app_onpowerup_hsl_reset();
extern void run_goo_client_ts(int argc, char *args[]);
extern void run_client_ts(int argc, char *args[]);
#endif

static uint16_t get_ele_addr(uint16_t dst,uint16_t ele_idx)
{
    if(dst == 0)
        aw_mesh_primary_addr_get(&dst);

    return dst + ele_idx;
}

static uint8_t char2int(uint8_t a)
{
	if (a >= '0' && a <= '9')
		return a - '0';
	else if (a >= 'a' && a <= 'f')
		return a - 'a' + 10;
	else if (a >= 'A' && a <= 'F')
		return a - 'A' + 10;
	else
		return 0;
}

static uint8_t str2uuid(uint8_t *str_uuid, uint8_t *uuid)
{
	uint8_t i;
	uint8_t *p, *str;

	if (str_uuid == NULL)
		return -1;

	if (strlen(str_uuid) != 32)
		return -1;

	p = uuid;
	str = str_uuid;

	for (i = 0; i < 16; i++) {
		p[i] = char2int(str[2 * i]) * 16 +
			char2int(str[2 * i + 1]);
	}

	return 0;
}

static void cmd_model_reset(int argc, char *args[])
{
   // pts_app_reset();
}

static void cmd_model_onpowerup_reset(int argc, char *args[])
{
    //pts_app_onpowerup_reset();
}

static void cmd_model_onpowerup_hsl_reset(int argc, char *args[])
{
    //pts_app_onpowerup_hsl_reset();
}

static void cmd_pts_client(int argc, char *args[])
{
    uint8_t i = 0;
   // run_client_ts(argc,args);
}

/* demo ADD_ELEMENT for test*/
uint16_t ele_idx;

AW_MESH_ELEMENT_DATA_T demo_add_ele = {
	.location = 0,
	.element_index = &ele_idx
};

static void cmd_enable(int argc, char *args[])
{
	uint32_t role, feature;
    uint8_t m_uuid[16];

    if(strlen(args[2]) < sizeof(m_uuid)*2)
    {
       l_error("args[3]uuid[%d]=%s strlen not match uuid",strlen(args[2]),args[2]);
       return ;
    }
    str2hex(args[2],strlen(args[2]),m_uuid,sizeof(m_uuid));
    role = strtol(args[0], NULL, 16);
    feature = strtol(args[1], NULL, 16);
    aw_mesh_demo_device_init();
    //aw_mesh_add_element(0,&ele_idx);
	//mesh_set_model_data(&demo_gooc);
	//mesh_set_model_data(&demo_health_server);
	//mesh_set_model_data(&demo_health_client);
	//mesh_set_model_data(&demo_vendor_server);
    l_info("mesh enable role:%d,uuid:%s",role,args[2]);
    aw_mesh_enable(role, feature, m_uuid);
}

static void cmd_db(int argc, char *args[])
{
	l_info("show provision database:\n");
	aw_mesh_show_prov_db();
}

#define ACCESS_OPCODE_SIZE(opcode)              ((opcode) >= 0xc00000 ? 3 : ((opcode) >= 0x8000 ? 2 : 1))
#define ACCESS_PARAM_MAX_SIZE 380
#define VDN_OPCODE_SIZE 3
#define ACCESS_MAX_SIZE 380
/*
static void cmd_vendor_model_send(UINT32 opcode, UINT8 *param, UINT16 param_len)
{
    uint8_t buf[ACCESS_PARAM_MAX_SIZE];
    uint8_t opcode_size;
    AW_MESH_ACCESS_MESSAGE_TX_T vnd_mdl_msg;
    if((ACCESS_OPCODE_SIZE(opcode)) != VDN_OPCODE_SIZE)
        return ;

    if(param_len > (ACCESS_PARAM_MAX_SIZE - VDN_OPCODE_SIZE))
        return ;

    buf[0] = ((opcode) >> 16) & 0xff;
    buf[1] = ((opcode) >> 8) & 0xff;
    buf[2] = (opcode) & 0xff;

    vnd_mdl_msg.meta_data.appkey_index = 0;
    vnd_mdl_msg.meta_data.dst_addr  = 2;
    vnd_mdl_msg.meta_data.ele_idx   = 0;
    vnd_mdl_msg.meta_data.netkey_index = 0;
    vnd_mdl_msg.meta_data.ttl = 127;
    memcpy(&buf[VDN_OPCODE_SIZE],param,param_len);
    vnd_mdl_msg.data = &buf[0];
    vnd_mdl_msg.dlen = VDN_OPCODE_SIZE + param_len;
    aw_mesh_send_mesh_msg(&vnd_mdl_msg);
}
*/
static void cmd_vendor_model_data_send(int argc, char *args[])
{
    uint8_t buf[ACCESS_MAX_SIZE];
    uint8_t len;
    //uint16_t dst,ele_idx,netkey_idx,appkey_idx,ttl;
    AW_MESH_ACCESS_MESSAGE_TX_T vnd_mdl_msg;

    if(args[0] == NULL)
    {
        l_error("invail vendor data: %p",args[0]);
        return ;
    }

    len = strlen(args[0]);
    if((len == 0)||(len/2 > ACCESS_MAX_SIZE))
    {
        l_error("invail vendor data: %s len:%d",args[0],strlen(args[0]));
    }

    if(!str2hex(args[0],len,buf,len/2))
    {
        l_error("invail vendor data: %s len:%d",args[0],strlen(args[0]));
        return ;
    }

    l_info("%s,vender_data = %s",__func__,args[0]);
    vnd_mdl_msg.meta_data.dst_addr          = strtol(args[1], NULL, 16);
    vnd_mdl_msg.meta_data.ele_idx           = strtol(args[2], NULL, 10);
    vnd_mdl_msg.meta_data.netkey_index      = strtol(args[3], NULL, 10);
    vnd_mdl_msg.meta_data.appkey_index      = strtol(args[4], NULL, 10);
    vnd_mdl_msg.meta_data.ttl               = strtol(args[5], NULL, 10);
    vnd_mdl_msg.data                        = &buf[0];
    vnd_mdl_msg.data[0] |= 0xC0;
    vnd_mdl_msg.dlen                        = strlen(args[0])/2;

    if((vnd_mdl_msg.dlen >= 3)&&((vnd_mdl_msg.data[1] == 0x3d)&&(vnd_mdl_msg.data[2] == 0x06)))
    {
        if(vnd_mdl_msg.data[0] == 0xC1)
        {
#ifdef MEST_TEST_LOG_ENABLE
            mesh_test_log("%s[src_ele_idx %x dst %x len %d]",STR_APP_CLIENT_XRADIO_VENDOR_NAME_GET,vnd_mdl_msg.meta_data.ele_idx,   \
                vnd_mdl_msg.meta_data.dst_addr,vnd_mdl_msg.dlen);
#endif
        }
        else if(vnd_mdl_msg.data[0] == 0xC2)
        {
            if(vnd_mdl_msg.dlen > 3)
            {
                vnd_mdl_msg.data[vnd_mdl_msg.dlen+2] = '\0';
#ifdef MEST_TEST_LOG_ENABLE
                mesh_test_log("%s[src_ele_idx %x dst %x len %d name %s ]",STR_APP_CLIENT_XRADIO_VENDOR_NAME_SET,vnd_mdl_msg.meta_data.ele_idx,   \
                    vnd_mdl_msg.meta_data.dst_addr,vnd_mdl_msg.dlen,&vnd_mdl_msg.data[2]);
#endif
            }
        }
        else if(vnd_mdl_msg.data[0] == 0xC3)
        {
            if(vnd_mdl_msg.dlen > 3)
            {
                vnd_mdl_msg.data[vnd_mdl_msg.dlen+2] = '\0';
#ifdef MEST_TEST_LOG_ENABLE
                mesh_test_log("%s[src_ele_idx %x dst %x len %d name %s ]",STR_APP_CLIENT_XRADIO_VENDOR_NAME_SET_UNACK,vnd_mdl_msg.meta_data.ele_idx,   \
                    vnd_mdl_msg.meta_data.dst_addr,vnd_mdl_msg.dlen,&vnd_mdl_msg.data[2]);
#endif
            }
        }
    }
    aw_mesh_send_mesh_msg(&vnd_mdl_msg);
}
static void cmd_invite(int argc, char *args[])
{
	uint8_t *s_uuid;
	uint8_t uuid[16];
	uint32_t attentionDuration;
	s_uuid = args[0];
	attentionDuration = strtol(args[1], NULL, 10);
	if (str2uuid(s_uuid, uuid)) {
		l_error("uuid: %s illegal", s_uuid);
		return;
	}
	l_info("cmd_invite attentionDuration: %d", attentionDuration);
	print_packet("Prov UUID", uuid, 16);
	aw_mesh_prov_invite(uuid, 16, attentionDuration);
}

static void cmd_prov_node_cancel(int argc, char *args[])
{
	int32_t reason = strtol(args[0], NULL, 10);
	aw_mesh_prov_cancel(reason);
}

static void cmd_unprov_scan(int argc, char *args[])
{
	uint32_t start, duration;
	start = strtol(args[0], NULL, 16);
	duration = strtol(args[1], NULL, 16) / 16 * 10;
	l_info("%s: %d - %d",__func__,start, duration);
	aw_mesh_scan(start == 0 ? false : true,duration);
}

static void cmd_set_scan_para(int argc, char *args[])
{
	l_info("%s\n",__func__);
}

static void cmd_set_adv_para(int argc, char *args[])
{
	l_info("%s\n",__func__);
}

static void cmd_send_beacon(int argc, char *args[])
{
	uint16_t num, interval;
	num = strtol(args[0], NULL, 16);
	interval = strtol(args[1], NULL, 16);
	l_info("%s num %x interval %x",__func__,num,interval);
    aw_mesh_send_beacon(num, interval);
}

static void cmd_set_scan_enable(int argc, char *args[])
{
	uint32_t scan_enable;
	scan_enable = strtol(args[0], NULL, 16);
	l_info("%s scan_enable %x",__func__,scan_enable);
}

static void cmd_app_key_update(int argc, char *args[])
{
	uint16_t dst, net_idx, app_idx;

	dst = strtol(args[0], NULL, 16);
	net_idx = strtol(args[1], NULL, 16);
	app_idx = strtol(args[2], NULL, 16);
    {
        uint16_t opcode = AW_MESH_ACCESS_MSG_CONFIG_APPKEY_UPDATE;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_APPKEY_UPDATE_T appkey_update = {
		.netkey_index = net_idx,
		.appkey_index = app_idx,
		.appkey = local_new_app_key,
        };
        aw_mesh_node_cfg_app_key_update(meta_data,opcode,appkey_update);
   }
}

static void cmd_appkey_add(int argc, char *args[])
{
	uint16_t dst, net_idx, app_idx;
	dst = strtol(args[0], NULL, 16);
	net_idx = strtol(args[1], NULL, 16);
	app_idx = strtol(args[2], NULL, 16);
    {
        uint16_t opcode = AW_MESH_ACCESS_MSG_CONFIG_APPKEY_ADD;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_APPKEY_ADD_T appkey_add = {
		.netkey_index = net_idx,
		.appkey_index = app_idx,
		.appkey = local_app_key,
        };
        aw_mesh_node_cfg_app_key_add(meta_data,opcode,appkey_add);
   }
}

static void cmd_bind(int argc, char *args[])
{
    UINT32 mod_id;
	uint16_t dst, app_idx;
	uint8_t ele_idx;

	dst = strtol(args[0], NULL, 16);
	ele_idx = strtol(args[1], NULL, 16);
	mod_id = strtol(args[2], NULL, 16);
	app_idx = strtol(args[3], NULL, 16);
    l_info("%s cmd_bind model_id %x",__func__,mod_id);
    {
        uint16_t opcode = AW_MESH_ACCESS_MSG_CONFIG_MODEL_APP_BIND;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_MODEL_APP_BIND_T model_app_bind = {
             .element_addr = get_ele_addr(dst,ele_idx),
             .appkey_index = app_idx,
             .model_id = mod_id,
        };
        aw_mesh_node_cfg_model_app_bind(meta_data,opcode,model_app_bind);
    }
}

static void cmd_goo(int argc, char *argv[])
{
    uint16_t dst,appkey_idx,on_off;
    dst = strtol(argv[0], NULL, 16);
    appkey_idx = strtol(argv[1], NULL, 16);
    on_off = strtol(argv[2], NULL, 16);
    aw_mesh_model_goo_send(dst,appkey_idx,on_off);
    l_info("%s\n",__func__);
}

static void cmd_goo_client(int argc, char *argv[])
{
    AW_MESH_SIG_MDL_MSG_TX_T sigMdl_tx_msg;
    uint16_t ele_idx,dst,appkey_idx,on_off;
    static uint8_t tid = 0;
    ele_idx = strtol(argv[0], NULL, 16);
    dst = strtol(argv[1], NULL, 16);
    appkey_idx = strtol(argv[2], NULL, 16);
    on_off = strtol(argv[3], NULL, 16);

    sigMdl_tx_msg.meta.src_element_index = ele_idx;
    sigMdl_tx_msg.meta.model_id = AW_MESH_SIG_MODEL_ID_GENERIC_ONOFF_CLIENT;
    sigMdl_tx_msg.meta.dst_addr = dst;
    sigMdl_tx_msg.meta.msg_appkey_index = appkey_idx;
    sigMdl_tx_msg.meta.tx_type = AW_MESH_MODEL_SEND;
    sigMdl_tx_msg.opcode = AW_MESH_MSG_GENERIC_ONOFF_SET;

    sigMdl_tx_msg.data.goo_set.b_ack = true;
    sigMdl_tx_msg.data.goo_set.b_trans_present = false;
    sigMdl_tx_msg.data.goo_set.target_onoff = on_off;
    sigMdl_tx_msg.data.goo_set.tid = tid;

    aw_mesh_send_sig_model_msg(&sigMdl_tx_msg);
    tid++;
    l_info("%s goo set tid %d\n",__func__,tid);
}

static void cmd_netkey_update(int argc, char *args[])
{
	uint16_t dst, net_idx, app_idx;
	uint8_t net_key[16] = {0x99, 0x96, 0x47, 0x71, 0x73, 0x4f, 0xbd, 0x76,
				0xe3, 0xb4, 0x05, 0x19, 0xd1, 0xd9, 0x4a, 0x48};

	dst = strtol(args[0], NULL, 16);
	net_idx = strtol(args[1], NULL, 16);
    {
        uint16_t opcode = OP_NETKEY_UPDATE;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,net_idx);
        AW_MESH_CONFIG_NETKEY_UPDATE_T netkey_update = {
            .netkey_index = net_idx,
            .netkey = net_key,
        };
        aw_mesh_node_cfg_net_key_update(meta_data,opcode,netkey_update);
    }
}

static void cmd_netkey_add(int argc, char *args[])
{
	uint16_t dst, net_idx, app_idx;
	uint8_t net_key[16] = {0x88, 0x96, 0x47, 0x71, 0x73, 0x4f, 0xbd, 0x76,
				0xe3, 0xb4, 0x05, 0x19, 0xd1, 0xd9, 0x4a, 0x48};

	dst = strtol(args[0], NULL, 16);
	net_idx = strtol(args[1], NULL, 16);
    {
        uint16_t opcode = OP_NETKEY_ADD;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,net_idx);
        AW_MESH_CONFIG_NETKEY_ADD_T netkey_add = {
            .netkey_index = net_idx,
            .netkey = net_key,
        };
        aw_mesh_node_cfg_net_key_add(meta_data,opcode,netkey_add);
    }
}

static void cmd_netkey_get(int argc, char *args[])
{
	uint16_t dst, net_idx;

	dst = strtol(args[0], NULL, 16);
	net_idx = strtol(args[1], NULL, 16);
    {
        uint16_t opcode = OP_NETKEY_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,net_idx);
        AW_MESH_CONFIG_NETKEY_GET_T netkey_get;
        aw_mesh_node_cfg_net_key_get(meta_data,opcode,netkey_get);
    }
}

static void cmd_netkey_del(int argc, char *args[])
{
	uint16_t dst, net_idx;

	dst = strtol(args[0], NULL, 16);
	net_idx = strtol(args[1], NULL, 16);
    {
        uint16_t opcode = OP_NETKEY_DELETE;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,net_idx);
        AW_MESH_CONFIG_NETKEY_DEL_T netkey_del= {
		.netkey_index = net_idx,
	    };
        aw_mesh_node_cfg_net_key_delete(meta_data,opcode,netkey_del);
    }
}

static void cmd_appkey_get(int argc, char *args[])
{
	uint16_t dst, net_idx;

	dst = strtol(args[0], NULL, 16);
	net_idx = strtol(args[1], NULL, 16);
    {
        uint16_t opcode = OP_APPKEY_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,net_idx);
        AW_MESH_CONFIG_APPKEY_GET_T appkey_get= {
		.netkey_index = net_idx,
	    };
        aw_mesh_node_cfg_app_key_get(meta_data,opcode,appkey_get);
    }
}

static void cmd_appkey_del(int argc, char *args[])
{
	uint16_t dst, net_idx, app_idx;

	dst = strtol(args[0], NULL, 16);
	net_idx = strtol(args[1], NULL, 16);
	app_idx = strtol(args[2], NULL, 16);
    {
        uint16_t opcode = OP_APPKEY_DELETE;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,net_idx);
        AW_MESH_CONFIG_APPKEY_DEL_T appkey_del= {
            .netkey_index = net_idx,
            .appkey_index = app_idx,
        };
        aw_mesh_node_cfg_app_key_delete(meta_data,opcode,appkey_del);
    }
}

static void cmd_unbind(int argc, char *args[])
{
	uint16_t dst, element_idx, app_idx;
	UINT32 model_id;

	dst = strtol(args[0], NULL, 16);
	element_idx = strtol(args[1], NULL, 16);
	app_idx = strtol(args[2], NULL, 16);
	model_id = strtol(args[3], NULL, 16);

    {
        uint16_t opcode = OP_MODEL_APP_UNBIND;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_MODEL_APP_UNBIND_T model_app_unbind= {
            .element_addr = get_ele_addr(dst,element_idx),
            .appkey_index = app_idx,
            .model_id = model_id,
        };
        aw_mesh_node_cfg_model_app_unbind(meta_data,opcode,model_app_unbind);
    }

}

static void cmd_model_app_get(int argc, char *args[])
{
	uint16_t dst, ele_idx, app_idx;
	UINT32 model_id;


	dst = strtol(args[0], NULL, 16);
	ele_idx = strtol(args[1], NULL, 16);
	model_id = strtol(args[2], NULL, 16);
    {
        uint16_t opcode = OP_MODEL_APP_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_SIG_MODEL_APP_GET_T sig_model_app_get= {
            .element_addr = get_ele_addr(dst,ele_idx),
            .model_id = model_id,
        };
        aw_mesh_node_cfg_model_app_get(meta_data,opcode,sig_model_app_get);
    }
}

static void cmd_vend_model_app_get(int argc, char *args[])
{
	uint16_t dst, ele_idx, app_idx;
	UINT32 model_id;
	dst = strtol(args[0], NULL, 16);
	ele_idx = strtol(args[1], NULL, 16);
	model_id = strtol(args[2], NULL, 16);
    {
        uint16_t opcode = OP_VEND_MODEL_APP_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_VENDOR_MODEL_APP_GET_T vendor_model_app_get= {
            .element_addr = get_ele_addr(dst,ele_idx),
            .model_id = model_id,
        };
        aw_mesh_node_cfg_vnd_app_get(meta_data,opcode,vendor_model_app_get);
    }
}

static void cmd_sub_set(int argc, char *args[])
{
	uint16_t dst, value, ele_idx, addr_type, opcode, ele_addr;
	UINT32 model_id;
	uint8_t virtual_uuid[16] = {0x66, 0x88, 0x47, 0x71, 0x73, 0x4f, 0xbd, 0x76,
			0xe3, 0xb4, 0x05, 0x19, 0xd1, 0xd9, 0x4a, 0x48};
	dst = strtol(args[0], NULL, 16);
	ele_idx = strtol(args[1], NULL, 16);
	value = strtol(args[2], NULL, 16);
	addr_type = strtol(args[3], NULL, 16);
	model_id = strtol(args[4], NULL, 16);
	opcode = strtol(args[5], NULL, 16);

    ele_addr = get_ele_addr(dst,ele_idx);

    {
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_ADDRESS_T address = {
            .type = addr_type,
            .value = value,
            .virtual_uuid = NULL,
        };
        AW_MESH_ADDRESS_T address_virtual = {
            .type = addr_type,
            .value = value,
            .virtual_uuid = virtual_uuid,
        };
        switch(opcode)
        {
            case OP_CONFIG_MODEL_SUB_ADD:
                {
                    AW_MESH_CONFIG_MODEL_SUB_ADD_T model_sub_add = {
                        .element_addr = ele_addr,
                        .address = address,
                        .model_id = model_id,
                    };
                    aw_mesh_node_cfg_model_sub_add(meta_data,opcode,model_sub_add);
                }
                break;
            case OP_CONFIG_MODEL_SUB_DELETE:
                {
                    AW_MESH_CONFIG_MODEL_SUB_DEL_T model_sub_del = {
                        .element_addr = ele_addr,
                        .address = address,
                        .model_id = model_id,
                    };
                    aw_mesh_node_cfg_model_sub_delete(meta_data,opcode,model_sub_del);
                }
                break;
            case OP_CONFIG_MODEL_SUB_OVERWRITE:
                {
                    AW_MESH_CONFIG_MODEL_SUB_OW_T model_sub_ow = {
                        .element_addr = ele_addr,
                        .address = address,
                        .model_id = model_id,
                    };
                    aw_mesh_node_cfg_model_sub_overwrite(meta_data,opcode,model_sub_ow);
                }

                break;
            case OP_CONFIG_MODEL_SUB_DELETE_ALL:
                {
                    AW_MESH_CONFIG_MODEL_SUB_DEL_ALL_T model_sub_del_all = {
                        .element_addr = ele_addr,
                        .model_id = model_id,
                    };
                    aw_mesh_node_cfg_model_sub_delete_all(meta_data,opcode,model_sub_del_all);
                }

                break;
            case OP_CONFIG_MODEL_SUB_VIRT_ADD:
                {
                    AW_MESH_CONFIG_MODEL_SUB_ADD_T model_sub_add = {
                        .element_addr = ele_addr,
                        .address = address_virtual,
                        .model_id = model_id,

                    };
                    aw_mesh_node_cfg_model_sub_add(meta_data,opcode,model_sub_add);
                }
                break;
            case OP_CONFIG_MODEL_SUB_VIRT_DELETE:
                {
                    AW_MESH_CONFIG_MODEL_SUB_DEL_T model_sub_del = {
				.element_addr = ele_addr,
				.address = address_virtual,
				.model_id = model_id,
                    };
                    aw_mesh_node_cfg_model_sub_delete(meta_data,opcode,model_sub_del);
                }
                break;
            case OP_CONFIG_MODEL_SUB_VIRT_OVERWRITE:
                {
                    AW_MESH_CONFIG_MODEL_SUB_OW_T model_sub_ow = {
                        .element_addr = ele_addr,
                        .address = address_virtual,
                        .model_id = model_id,

                    };
                    aw_mesh_node_cfg_model_sub_overwrite(meta_data,opcode,model_sub_ow);
                }
                break;
            default:
                l_debug("opcode 0x%x error", opcode);
        }
    }
}

static void cmd_sub_get(int argc, char *args[])
{
	uint16_t dst, ele_idx,ele_addr;
	int16_t is_vendor;
	UINT32 model_id;
	dst = strtol(args[0], NULL, 16);
	ele_idx = strtol(args[1], NULL, 16);
	model_id = strtol(args[2], NULL, 16);
	is_vendor = strtol(args[3], NULL, 16);
    ele_addr = get_ele_addr(dst,ele_idx);
    {
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        switch(is_vendor)
        {
            case 1:
                {
                    uint16_t opcode = OP_CONFIG_VEND_MODEL_SUB_GET;
                    AW_MESH_CONFIG_VENDOR_MODEL_SUB_GET_T vendor_model_sub_get = {
                        .element_addr = ele_addr,
                        .model_id = model_id,
                    };
                    aw_mesh_node_cfg_vnd_sub_get(meta_data,opcode,vendor_model_sub_get);
                }
                break;
            default:
                {
                    uint16_t opcode = OP_CONFIG_MODEL_SUB_GET;
                    AW_MESH_CONFIG_SIG_MODEL_SUB_GET_T sig_model_sub_get = {
                        .element_addr = ele_addr,
                        .model_id = (uint16_t) model_id,
                    };
                    aw_mesh_node_cfg_model_sub_get(meta_data,opcode,sig_model_sub_get);
                }
                break;
        }
    }
}

static void cmd_pub_set(int argc, char *args[])
{
	uint16_t dst, addr, app_idx, ele_idx, addr_type;
	UINT32 model_id;
	uint8_t period, ttl, count, step;
	uint8_t virtual_uuid[16] = {0x77, 0x77, 0x47, 0x71, 0x73, 0x4f, 0xbd, 0x76,
				0xe3, 0xb4, 0x05, 0x19, 0xd1, 0xd9, 0x4a, 0x48};

	dst = strtol(args[0], NULL, 16);
	ele_idx = strtol(args[1], NULL, 16);
	addr = strtol(args[2], NULL, 16);
	addr_type = strtol(args[3], NULL, 16); /*0 is normal  1 is virtual*/
	model_id = strtol(args[4], NULL, 16);
	app_idx = strtol(args[5], NULL, 16);
	period = strtol(args[6], NULL, 16);
	ttl = strtol(args[7], NULL, 16);
	count = strtol(args[8], NULL, 16);
	step = strtol(args[9], NULL, 16);

    {
        /*MESH/CFGCL/CFG/MP/BV-01-C*/
        uint16_t opcode ;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,0x3f,0);
        AW_MESH_ADDRESS_T address = {
            .type = AW_MESH_ADDRESS_TYPE_UNICAST,
            .value = addr,
            .virtual_uuid = NULL,
        };
        AW_MESH_ADDRESS_T address_vir = {
            .type = AW_MESH_ADDRESS_TYPE_VIRTUAL,
            .value = 0,
            .virtual_uuid = virtual_uuid,
        };

        AW_MESH_MODEL_PUBLICATION_STATE_T state = {
            .element_address = get_ele_addr(dst,ele_idx),
            .publish_address = address,
            .appkey_index = app_idx,
            .friendship_credential_flag = false,
            .publish_ttl = ttl,
            .publish_period = period,
            .retransmit_count = count,
            .retransmit_interval_steps = step,
            .model_id = model_id,
        };
        AW_MESH_CONFIG_MODEL_PUB_SET_T model_pub_set = {
            .state = &state,
        };
        if(addr_type == 1)
        {
            opcode = OP_CONFIG_MODEL_PUB_VIRT_SET;
            model_pub_set.state->publish_address = address_vir;
        }
        else
        {
            opcode = OP_CONFIG_MODEL_PUB_SET;
        }
        aw_mesh_node_cfg_model_pub_set(meta_data,opcode,model_pub_set);
    }
}


static void cmd_pub_get(int argc, char *args[])
{

	uint16_t dst, ele_idx, app_idx;
	UINT32 model_id;
	dst = strtol(args[0], NULL, 16);
	ele_idx = strtol(args[1], NULL, 16);
	model_id = strtol(args[2], NULL, 16);
    {
        uint16_t opcode = OP_CONFIG_MODEL_PUB_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_MODEL_PUB_GET_T model_pub_get = {
            .element_addr = get_ele_addr(dst,ele_idx),
            .model_id = model_id,
        };
        aw_mesh_node_cfg_model_pub_get(meta_data,opcode,model_pub_get);
    }
}


static void cmd_composition_get(int argc, char *args[])
{
	uint16_t dst;
	uint8_t page;

	dst = strtol(args[0], NULL, 16);
	page = strtol(args[1], NULL, 16);
    {
        uint16_t opcode = OP_DEV_COMP_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_COMPOSITION_DATA_GET_T composition_data_get = {
            .page = page,
        };
        aw_mesh_node_cfg_compo_data_get(meta_data,opcode,composition_data_get);
    }

}
static void cmd_beacon_set(int argc, char *args[])
{
	uint16_t dst;
	uint8_t beacon;

	dst = strtol(args[0], NULL, 16);
	beacon = strtol(args[1], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_BEACON_SET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_BEACON_SET_T beacon_set = {
            .beacon = beacon,
        };
        aw_mesh_node_cfg_beacon_set(meta_data,opcode,beacon_set);
    }

}

static void cmd_beacon_get(int argc, char *args[])
{
	uint16_t dst;

	dst = strtol(args[0], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_BEACON_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_BEACON_GET_T beacon_get ;
       aw_mesh_node_cfg_beacon_get(meta_data,opcode,beacon_get);
    }

}

static void cmd_relay_set(int argc, char *args[])
{
	uint16_t dst;
	uint8_t relay, retrans_count, retrans_steps;

	dst = strtol(args[0], NULL, 16);
	relay = strtol(args[1], NULL, 16);
	retrans_count = strtol(args[2], NULL, 16);
	retrans_steps = strtol(args[3], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_RELAY_SET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_RELAY_SET_T relay_set = {
            .relay = relay,
            .retransmit_count = retrans_count,
            .retransmit_interval_steps = retrans_steps,
        };
        aw_mesh_node_cfg_relay_set(meta_data,opcode,relay_set);
    }

}

static void cmd_relay_get(int argc, char *args[])
{
	uint16_t dst;

	dst = strtol(args[0], NULL, 16);
    {
        uint16_t opcode = OP_CONFIG_RELAY_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_RELAY_GET_T relay_get ;
        aw_mesh_node_cfg_relay_get(meta_data,opcode,relay_get);
    }

}

static void cmd_ident_set(int argc, char *args[])
{
	uint16_t dst, netkey_idx;
	uint8_t identitys;


	dst = strtol(args[0], NULL, 16);
	netkey_idx = strtol(args[1], NULL, 16);
	identitys = strtol(args[2], NULL, 16);

    {
        uint16_t opcode = OP_NODE_IDENTITY_SET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_NODE_IDENTITY_SET_T node_identity_set = {
            .netkey_index = netkey_idx,
            .identity = identitys,
        };
        aw_mesh_node_cfg_node_identity_set(meta_data,opcode,node_identity_set);
    }

}
static void cmd_ident_get(int argc, char *args[])
{
	uint16_t dst, netkey_idx;

	dst = strtol(args[0], NULL, 16);
	netkey_idx = strtol(args[1], NULL, 16);

    {
        uint16_t opcode = OP_NODE_IDENTITY_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_NODE_IDENTITY_GET_T node_identity_get = {
            .netkey_index = netkey_idx,
        };
        aw_mesh_node_cfg_node_identity_get(meta_data,opcode,node_identity_get);
    }

}

static void cmd_proxy_set(int argc, char *args[])
{
	uint16_t dst;
	uint8_t proxy;

	dst = strtol(args[0], NULL, 16);
	proxy = strtol(args[1], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_PROXY_SET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_GATT_PROXY_SET_T gatt_proxy_set = {
            .gatt_proxy = proxy,
        };
        aw_mesh_node_cfg_proxy_set(meta_data,opcode,gatt_proxy_set);
    }

}

static void cmd_proxy_get(int argc, char *args[])
{
	uint16_t dst;
	uint8_t proxy;

	dst = strtol(args[0], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_PROXY_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_GATT_PROXY_GET_T gatt_proxy_get ;
        aw_mesh_node_cfg_proxy_get(meta_data,opcode,gatt_proxy_get);
    }

}
static void cmd_hb_sub_add(int argc, char *args[])
{
	uint16_t dst, source, dst_addr;
	uint8_t period;

	dst = strtol(args[0], NULL, 16);
	source = strtol(args[1], NULL, 16);
	dst_addr = strtol(args[2], NULL, 16);
	period = strtol(args[3], NULL, 16);
    {
        uint16_t opcode = OP_CONFIG_HEARTBEAT_SUB_SET;
        AW_MESH_HEARTBEAT_SUBSCRIPTION_T subscription = {
            .source = source,
            .destination = dst_addr,
            .period_log = period
        };

        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_HB_SUB_SET_T hb_sub_set = {
            .subscription = &subscription,
        };
        aw_mesh_node_cfg_hb_sub_set(meta_data,opcode,hb_sub_set);
    }

}

static void cmd_hb_sub_get(int argc, char *args[])
{
	uint16_t dst;
	uint8_t proxy;

	dst = strtol(args[0], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_HEARTBEAT_SUB_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_HB_SUB_GET_T hb_sub_get ;
        aw_mesh_node_cfg_hb_sub_get(meta_data,opcode,hb_sub_get);
    }

}

static void cmd_hb_pub_add(int argc, char *args[])
{
	uint16_t dst, pub_dst, netkey_idx, features;
	uint8_t peroid, count_log, ttl;

	dst = strtol(args[0], NULL, 16);
	pub_dst = strtol(args[1], NULL, 16);
	count_log = strtol(args[2], NULL, 16);
	peroid = strtol(args[3], NULL, 16);
	ttl = strtol(args[4], NULL, 16);
	features = strtol(args[5], NULL, 16);
	netkey_idx = strtol(args[6], NULL, 16);
    {
        uint16_t opcode = OP_CONFIG_HEARTBEAT_PUB_SET;
        AW_MESH_HEARTBEAT_PUBLICATION_T publication = {
            .destination = pub_dst,
            .count_log = count_log,
            .period_log = peroid,
            .ttl = ttl,
            .features = features,
            .netkey_index = netkey_idx,
        };
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_HB_PUB_SET_T hb_pub_set = {
            .publication = &publication,
        };
        aw_mesh_node_cfg_hb_pub_set(meta_data,opcode,hb_pub_set);
    }

}

static void cmd_hb_pub_get(int argc, char *args[])
{
	uint16_t dst;

	dst = strtol(args[0], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_HEARTBEAT_PUB_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_HB_PUB_GET_T hb_pub_get ;
        aw_mesh_node_cfg_hb_pub_get(meta_data,opcode,hb_pub_get);
    }

}

static void cmd_ttl_set(int argc, char *args[])
{
	uint16_t dst;
	uint8_t ttl;

	dst = strtol(args[0], NULL, 16);
	ttl = strtol(args[1], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_DEFAULT_TTL_SET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_DEFAULT_TTL_SET_T default_ttl_set = {
		.meta = meta_data,
		.TTL = ttl,
	    };
        aw_mesh_node_cfg_ttl_set(meta_data,opcode,default_ttl_set);
    }

}
static void cmd_ttl_get(int argc, char *args[])
{
	uint16_t dst;

	dst = strtol(args[0], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_DEFAULT_TTL_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_DEFAULT_TTL_GET_T default_ttl_get ;
        aw_mesh_node_cfg_ttl_get(meta_data,opcode,default_ttl_get);
    }
}

static void cmd_net_transmit_set(int argc, char *args[])
{
	uint16_t dst;
	uint8_t count, steps;

	dst = strtol(args[0], NULL, 16);
	count = strtol(args[1], NULL, 16);
	steps = strtol(args[2], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_NETWORK_TRANSMIT_SET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_NETWORK_TRANSMIT_SET_T net_trans_set = {
            .count = count,
            .interval_steps = steps,
        };
        aw_mesh_node_cfg_transmit_set(meta_data,opcode,net_trans_set);
    }

}
static void cmd_net_transmit_get(int argc, char *args[])
{
	uint16_t dst;

	dst = strtol(args[0], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_NETWORK_TRANSMIT_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_NETWORK_TRANSMIT_GET_T net_trans_get ;
        aw_mesh_node_cfg_transmit_get(meta_data,opcode,net_trans_get);
    }

}

static void cmd_friend_get(int argc, char *args[])
{
	uint16_t dst;

	dst = strtol(args[0], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_FRIEND_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_FRIEND_GET_T friend_get ;
        aw_mesh_node_cfg_frnd_get(meta_data,opcode,friend_get);
    }

}

static void cmd_friend_set(int argc, char *args[])
{
	uint16_t dst;
	uint8_t enable;

	dst = strtol(args[0], NULL, 16);
	enable = strtol(args[1], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_FRIEND_SET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_FRIEND_SET_T friend_set = {
            .mesh_friend = enable,
        };
        aw_mesh_node_cfg_frnd_set(meta_data,opcode,friend_set);
    }

}


static void cmd_network_layer_test(int argc, char *args[])
{
	uint16_t dst;

	dst = strtol(args[0], NULL, 16);
	uint8_t data[15] = {2, 3, 4, 5, 6, 7, 8 ,9, 10, 11, 12, 13, 14, 15 ,16};
	l_info("==========dst %x", dst);
	aw_mesh_send_packet(dst, 0, 0, 0, 0, 0x7fff, data, sizeof(data));
    aw_mesh_send_packet(dst, 0, 0, 0, 0, 0x7fff, data, sizeof(data));
	usleep(1000 * 200);
	aw_mesh_send_packet(dst, 0, 0, 10, 0, 0x7fff, data, sizeof(data));
	usleep(1000 * 200);
	aw_mesh_send_packet(dst, 0, 0, 127, 0, 0x7fff, data, sizeof(data));

	return;
}

static void cmd_node_reset(int argc, char *args[])
{
	uint16_t dst;
	dst = strtol(args[0], NULL, 16);

    {
        uint16_t opcode = OP_NODE_RESET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_NODE_RESET_T node_reset ;
        aw_mesh_node_cfg_node_reset(meta_data,opcode,node_reset);
    }

}

static void cmd_poll_timeout(int argc, char *args[])
{
	uint16_t dst, address;

	dst = strtol(args[0], NULL, 16);
	address = strtol(args[1], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_POLL_TIMEOUT_LIST;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_POLL_TIMEOUT_T poll_timeout = {
            .address = address,
        };
        aw_mesh_node_cfg_lpn_poll_timeout_get(meta_data,opcode,poll_timeout);
    }

}

static void cmd_set_test_data(int argc, char *args[])
{
	uint16_t mode;

	/*MESH/NODE/PROV/BI-02-03-C BV-10-C value 0x50	test finish set_test_mode 0*/
	/*MESH/PVNR/PBADV/BV-01-C	   value 0x60*/

	/*MESH/NODE/IVU/BV-05-C value is 2 */
	/*MESH/NODE/IVU/BI-06-C value is 3 */
	/*MESH/NODE/IV/BV-04-C value	0x4*/
	/*MESH/NODE/TNPT/BV-12-C value is 1 */
	/*MESH/SR/HM/CFG/ value is 5 clear period timeout*/
	/*MESH/NODE/IVU/BV-02-C value is 6 set ivu update  updating*/
	/*MESH/NODE/IVU/BV-01-02-C value is 7 set ivu update  normal*/
	/*MESH/NODE/RLY/BV-02-C value is 0xf*/
	/*MESH/NODE/FRND/LPN     value 0x10   low power poll message*/
	/*MESH/NODE/FRND/LPN/BV-04-C      value 0x11 low power send friend subscription list add message*/
	/*MESH/NODE/FRND/LPN/BV-05-C value 0x12      low power send friend subscription list delete message  1.first send sub add 2. second send sub del*/
	/*MESH/NODE/FRND/LPN/BV-08-C value 0x13      low power send friend clear message    */
	/*MESH/NODE/FRND/LPN/BV-02-C value 0x14      low power send health status message with master security credentials*/
	/*MESH/NODE/FRND/LPN/BV-02-C value 0x15      low power send health status message with friendship security credentials*/

	/*MESH/NODE/FRND/FN/BV-04-C value 0x20 set iv update 0x0a iv update normal  set_test_mode 0xfe  set_test_mode 0xff*/
	/*MESH/NODE/FRND/FN/BV-08-C value 0x21 set frnd->pkt_cache 5  , value 0x 27 set frnd->pkt_cache 7. set_test_mode 0x21  set_test_mode 0x27 set_test_mode 0*/
	/*MESH/NODE/FRND/FN/BV-12-C      value 0x28 */
	/*MESH/NODE/FRND/FN/BV-20-C value 0x22 friend node enqueue secure network beacon iv flag 1 key refresh 0 and iv flag 1 key refresh 1*/
	/*MESH/NODE/FRND/FN/BV-14-C      value 0x25 set node kr flag 1  0x26 set node kr flag 0  0x2b just for test to force enable net_update_key*/
	/*MESH/NODR/FRND/FN/BV-19-C value 0x29 */
	/*MESH/NODE/FRND/FN/BI-02-C value 0x2a*/

	/*MESH/NODE/CFG/ value 0x70*/
	/*MESH/NODE/CFG/DTTL/BI-01-C value 0x71*/
	/*MESH/NODE/CFG/AKL/BI-04-C value 0x72*/
	/*MESH/NODE/CFG/HBP/BV-06-C       value 0x73*/
	/*MESH/NODE/CFG/HBP/BV-05-C value 0x74*/
	/*MESH/NODE/CFG/NKL/BV-03-C value 0x90*/
	/*MESH/NODE/CFG/NKL/BI-03-C value 0x91*/
	/*MESH/NODE/CFG/HBS/BV-04-C value 0x94*/
	/*MESH/NODE/CFG/HBS/BV-02-C       BV-03-C value 0x95*/
	/*MESH/NODE/TNPT/BV-09-C value 0x92*/
	/*MESH/NODE/TNPT/BV-07-C value 0x93*/
	/*MESH/NODE/NET/BV-07-C value 0x96*/

	/*MESH/NODE/KR/BV-01-C BV-03-C value 0x75*/
	/*MESH/NODE/KR/BI-01-C     value 0x76*/
	/*MESH/SR/HM/CFS/BV-02-C value 0xf0*/
	mode = strtol(args[0], NULL, 16);

}



static void cmd_node_set_mode(int argc, char *args[])
{
	uint16_t mode;

	/*MESH/NODE/PROV/BV-01-C       no public key output oob 0x51*/
	/*MESH/NODE/PROV/BV-02-C       no public key input oob 0x52*/
	/*MESH/NODE/PROV/BV-03-C       no public key static oob 0x53*/
	/*MESH/NODE/PROV/BV-04-C       public key output oob 0x54*/
	/*MESH/NODE/PROV/BV-05-C       public key input oob 0x55*/
	/*MESH/NODE/PROV/BV-06-C       public key static oob 0x56*/
	/*MESH/NODE/PROV/BV-07-C       no public key no oob 0x57*/
	/*MESH/NODE/PROV/BV-08-C       public key no oob 0x58*/
	mode = strtol(args[0], NULL, 16);
}

static void cmd_set_health_test(int argc, char *args[])
{
	uint8_t mode, data, ack;
	uint16_t dst, app_idx;

	/*
		mode:
		1: fautl_get
		2:fault_clear ack
		3:fault_clear unack
		4:fault_test ack
		5:fault_test unack
		6:period_get
		7:period_set ack
		8:period_set unack
		9:atten_get
		10:atten_set ack
		11:atten_set unack
	*/

	mode = strtol(args[0], NULL, 16);
	dst = strtol(args[1], NULL, 16);
	app_idx = strtol(args[2], NULL, 16);
	data = strtol(args[3], NULL, 16);
	ack = strtol(args[4], NULL, 16);
	/*data :1  ack 1/0 */
}


static void cmd_send_data(int argc, char *args[])
{
	uint16_t dst, address, group_addr;
	uint8_t segmented, group_or_virtual;
	uint8_t data_unseg[5] = {2, 3, 4, 5, 6};
	uint8_t data_seg[16] = {2, 3, 4, 5, 6, 3, 4, 5, 6, 3, 4, 5, 6, 2, 3, 4};

	dst = strtol(args[0], NULL, 16);
	segmented = strtol(args[1], NULL, 16);
	group_addr = strtol(args[2], NULL, 16);

	if (segmented == 1 && group_addr != 0) {
		aw_mesh_send_packet(group_addr, 0, 0, 127, 0, 0, data_seg, sizeof(data_seg));
	} else if (segmented == 1 && group_addr == 0) {
		aw_mesh_send_packet(dst, 0, 0, 127, 0, 0, data_seg, sizeof(data_seg));
	} else {
		aw_mesh_send_packet(dst, 0, 0, 127, 0, 0, data_unseg, sizeof(data_unseg));
	}
}

static void cmd_key_refresh_set(int argc, char *args[])
{
	uint16_t dst, netkey_idx;
	uint8_t transition;
	dst = strtol(args[0], NULL, 16);
	netkey_idx = strtol(args[1], NULL, 16);
	transition = strtol(args[2], NULL, 16);

    {
        uint16_t opcode = OP_CONFIG_KEY_REFRESH_PHASE_SET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_KEY_REFRESH_PHASE_SET_T key_ref_pha_set = {
            .netkey_index = netkey_idx,
            .transition = transition,
        };
        aw_mesh_node_cfg_key_refresh_phase_set(meta_data,opcode,key_ref_pha_set);
    }
}

static void cmd_key_refresh_get(int argc, char *args[])
{
	uint16_t dst, netkey_idx;
	uint8_t transition;

	dst = strtol(args[0], NULL, 16);
	netkey_idx = strtol(args[1], NULL, 16);
    {
        uint16_t opcode = OP_CONFIG_KEY_REFRESH_PHASE_GET;
        AW_MESH_BUILD_CFG_META_DATA(meta_data,dst,255,0);
        AW_MESH_CONFIG_KEY_REFRESH_PHASE_GET_T key_ref_pha_get = {
            .netkey_index = netkey_idx,
        };
        aw_mesh_node_cfg_key_refresh_phase_get(meta_data,opcode,key_ref_pha_get);
    }
}

static void cmd_set_start_beacon(int argc, char *args[])
{
	uint16_t dst, net_idx, phase;

	dst = strtol(args[0], NULL, 16);
	net_idx = strtol(args[1], NULL, 16);
	phase = strtol(args[2], NULL, 16);
	aw_mesh_set_start_beacon(dst, net_idx, phase);
}
static void cmd_dis_test(int argc, char *args[])
{
	uint32_t times, duration;

	times = strtol(args[0], NULL, 10);
	duration = strtol(args[1], NULL, 10);

	l_info("cmd_dis_test");

	if (times) {
		while (times-- > 0) {
			aw_mesh_scan(true, duration);
			sleep(duration + 2);
		}
	}
}


/*default cache:4 offer_delay:4 delay:160 timeou:300  default:0324a00003840000010001  cmd: friend_request 4 4 160 300*/
static void cmd_friend_request(int argc, char *args[]) {
	uint8_t cache, offer_delay, delay;
	uint32_t timeout;
	cache = strtol(args[0], NULL, 10);
	offer_delay = strtol(args[1], NULL, 10);
	delay = strtol(args[2], NULL, 10);;
	timeout = strtol(args[3], NULL, 10);

	aw_mesh_frnd_request_friend(cache, offer_delay, delay, timeout);
}

static void cmd_prov_set_manaual_choose(int argc, char *args[])
{
	uint8_t enable = 0;

	enable = strtol(args[0], NULL, 16);

	aw_mesh_prov_set_manaual_choose(enable);
}

static void cmd_prov_set_choose_param(int argc, char *args[])
{
	uint8_t pub_type, auth, auth_size;
	uint16_t auth_action;

	pub_type = strtol(args[0], NULL, 16);
	auth = strtol(args[1], NULL, 16);
	auth_size = strtol(args[2], NULL, 16);
	auth_action = strtol(args[3], NULL, 16);
	aw_mesh_prov_set_start_choose_paramters(pub_type, auth, auth_size, auth_action);
	return;
}

static void cmd_prov_set_static_val(int argc, char *args[])
{
	uint8_t static_val[16];
	uint8_t *static_auth;
	static_auth = args[0];
	printf("static val %s", static_auth);
	//static_auth = "00000000000000000102030405060708";
	if (str2uuid(static_auth, static_val)) {
		printf("static_auth: %s illegal", static_auth);
		return;
	}

	aw_mesh_prov_set_auth_static_value(static_val);

}

static void cmd_prov_set_auth_num(int argc, char *args[])
{
	uint8_t auth_num = 0;
	auth_num = strtol(args[0], NULL, 16);
	aw_mesh_prov_set_auth_number(auth_num);
}

static uint8_t str2pubkey(uint8_t *str_pubkey, uint8_t *pubkey)
{
	uint8_t i;
	uint8_t *p, *str;

	if (str_pubkey == NULL)
		return -1;

	if (strlen(str_pubkey) != 128)
		return -1;

	p = pubkey;
	str = str_pubkey;

	for (i = 0; i < 64; i++) {
		p[i] = char2int(str[2 * i]) * 16 +
			char2int(str[2 * i + 1]);
	}

	return 0;
}


static void cmd_prov_set_pub_key(int argc, char *args[])
{
	uint8_t pubkey[64];
	uint8_t *pub_key_s;

	pub_key_s = args[0];

	printf("public key %s\n", pub_key_s);
	if (str2pubkey(pub_key_s, pubkey)) {
		printf("pub_key_s: %s illegal\n", pub_key_s);
		return;
	}

	aw_mesh_prov_set_pub_key(pubkey);
}

static void cmd_prov_set_data(int argc, char *args[])
{
    uint16_t net_idx;
    uint16_t unicast;
    net_idx = strtol(args[0], NULL, 16);
    unicast = strtol(args[1], NULL, 16);
	aw_mesh_set_prov_data(net_idx,unicast);
}

static void cmd_mesh_set_protocol_param(int argc, char *args[])
{
	uint16_t crpl, prov_data_interval;
	crpl = strtol(args[0], NULL, 16);
	prov_data_interval = strtol(args[1], NULL, 16);

	printf("crpl %d prov_interval %d\n", crpl, prov_data_interval);

	aw_mesh_set_protocol_param(crpl, prov_data_interval,CONFIG_NODE_FEATURE);
}

static void cmd_mesh_set_iv_test_mode(int argc, char *args[])
{
    uint8_t testmode;
    testmode = strtol(args[0], NULL, 16);
    aw_mesh_iv_test_mode(testmode);
}

static void cmd_mesh_set_iv_index_update(int argc, char *args[])
{
    uint32_t iv_idx;
    uint8_t flag;
    iv_idx = strtol(args[0], NULL, 16);
    flag = strtol(args[1], NULL, 16);
    aw_mesh_update_iv_info(iv_idx,flag);
}

static void cmd_mesh_get_iv_index_update(int argc, char *args[])
{
    aw_mesh_get_iv_info();
}

static void cmd_quit_app(int argc, char *args[])
{
	l_info("cmd_quit_app");

	__main_terminated = 1;
}

cmd_tbl_t cmd_table[] = {
	{ "enable",		                NULL,	3,    cmd_enable,		"<role> <feature><uuid>"},
	{ "db",			                NULL,	0,    cmd_db,			""},
	{ "unprov_scan",	            NULL,	2,    cmd_unprov_scan,	"<start> <duration>"},
	{ "prov_node",		            NULL,	2,    cmd_invite,		"<uuid> <attentionDuration>"},
	{ "prov_cancel",	            NULL,	1,    cmd_prov_node_cancel,	"<reason>"},
	{ "scan",		                NULL,	1,    cmd_set_scan_enable,	"<scan_enable>"},
	{ "setScanPara",	            NULL,	2,    cmd_set_scan_para,	"<scan_interval> <scan_window>"},
	{ "setAdvPara",		            NULL,	3,    cmd_set_adv_para,	"<adv_interval_min> <adv_interval_max>"},
//	{ "goos",		NULL,	0,    cmd_goos,		"<dst> <times> <time_sum> <time_state> <resend> <delay>"},
	{ "goo",		                NULL,	3,    cmd_goo,		"<dst> <appkey> <on_off>"},
    { "goo_client",                 NULL,   4,    cmd_goo_client,      "<src_ele_idx><<dst> <appkey> <on_off>"},
	{ "dis-test",		            NULL,	2,    cmd_dis_test,		"<times> <duration>"},
	{ "quit",		                NULL,	0,    cmd_quit_app,		""},
	{ "netkey-add",		            NULL,	2,    cmd_netkey_add,		"<dst> <net_idx>"},
	{ "netkey-del",		            NULL,	2,    cmd_netkey_del,		"<dst> <net_idx>"},
	{ "netkey-get",		            NULL,	2,    cmd_netkey_get,		"<dst> <net_idx>"},
	{ "netkey-update",	            NULL,	2,    cmd_netkey_update,		"<dst> <net_idx>"},
	{ "appkey-add",		            NULL,	3,    cmd_appkey_add,	"<dst> <net_idx> <app_idx>"},
	{ "appkey-get",		            NULL,	2,    cmd_appkey_get,		"<dst> <net_idx>"},
	{ "appkey-del",		            NULL,	3,    cmd_appkey_del,		"<dst> <net_idx> <app_idx>"},
	{ "appkey-update",	            NULL,	3,    cmd_app_key_update,	"<dst> <net_idx> <app_idx>"},
	{ "bind",		                NULL,	4,    cmd_bind,		"<dst> <ele_idx> <model_id> <app_idx>"},
	{ "unbind",		                NULL,	4,    cmd_unbind,		"<dst> <ele_idx> <app_idx> <model_id>"},
	{ "model-app-get",	            NULL,	3,    cmd_model_app_get,	"<dst> <ele_idx> <model_id>"},
	{ "vend-model-app-get",	        NULL,	3,    cmd_vend_model_app_get,	"<dst> <ele_idx> <model_id>"},
	{ "sub-set",		            NULL,	6,    cmd_sub_set,		"<dst> <ele_idx> <address> <addr_type> <model_id> <opcode>"},
	{ "sub-get",		            NULL,	4,    cmd_sub_get,		"<dst> <ele_idx> <model_id> <is_vendor>"},
	{ "pub-set",		            NULL,  10,    cmd_pub_set,		"<dst> <ele_idx> <address> <addr_type> <mod_id> <app_idx> <period> <ttl> <count> <step>"},
	{ "pub-get",		            NULL,	3,    cmd_pub_get,		"<dst> <ele_idx> <model_id>"},
	{ "composition-get",	        NULL,	2,    cmd_composition_get,	"<dst> <page>"},
	{ "beacon-set",		            NULL,	2,    cmd_beacon_set,		"<dst> <beacon>"},
	{ "beacon-get",		            NULL,	1,    cmd_beacon_get,		"<dst>"},
	{ "relay-set",		            NULL,	4,    cmd_relay_set,		"<dst> <relay> <retrans_count> <retrans_steps>"},
	{ "relay-get",		            NULL,	1,    cmd_relay_get,		"<dst>"},
	{ "ident-set",		            NULL,	3,    cmd_ident_set,		"<dst> <net_idx> <identity>"},
	{ "ident-get",		            NULL,	2,    cmd_ident_get,		"<dst> <net_idx>"},
	{ "proxy-set",		            NULL,	2,    cmd_proxy_set,		"<dst> <proxy>"},
	{ "proxy-get",		            NULL,	1,    cmd_proxy_get,		"<dst>"},
	{ "hb-sub-set",		            NULL,	4,    cmd_hb_sub_add,		"<dst> <source> <dst_addr><period>"},
	{ "hb-sub-get",		            NULL,	1,    cmd_hb_sub_get,		"<dst>"},
	{ "hb-pub-set",		            NULL,	7,    cmd_hb_pub_add,		"<dst> <pub_dst> <count> <period> <ttl> <features> <net_idx>"},
	{ "hb-pub-get",		            NULL,	1,    cmd_hb_pub_get,		"<dst>"},
	{ "ttl-set",		            NULL,	2,    cmd_ttl_set,		"<dst> <ttl>"},
	{ "ttl-get",		            NULL,	1,    cmd_ttl_get,		"<dst>"},
	{ "net-transmit-set",		NULL,	3,    cmd_net_transmit_set,	"<dst> <count> <step>"},
	{ "net-transmit-get",		NULL,	1,    cmd_net_transmit_get,	"<dst>"},
	{ "friend-get",		            NULL,	1,    cmd_friend_get,		"<dst>"},
	{ "friend-set",		            NULL,	2,    cmd_friend_set,		"<dst> <enable>"},
  //  {"repeat",              NULL,   cmd_repeat_set,         "<dst>"},
    {"model_reset",                 NULL,   0,    cmd_model_reset,         "<dst>"},
    {"onpowerup_reset",             NULL,   0,    cmd_model_onpowerup_reset,         "<dst>"},
    {"onpowerup_hsl_reset",         NULL,   0,    cmd_model_onpowerup_hsl_reset,     "<dst>"},
    {"pts_client",                  NULL,   0,    cmd_pts_client,      "<ts_client> <case>"},
    { "pts_network_layer_test",     NULL,   1,    cmd_network_layer_test, "<test>"},
	{ "node_reset",		            NULL,	1,    cmd_node_reset,		"<dst>"},
	{ "poll-timeout",	            NULL,	2,    cmd_poll_timeout,	"<dst> <address>"},
	{ "send_data",		            NULL,	3,    cmd_send_data,		"<dst> <segmented> <group/virtual>"},
	{ "set_test_mode",	            NULL,	1,    cmd_set_test_data,		"<mode>"},
	{ "key-refresh-get",            NULL,	2,    cmd_key_refresh_get,	"<dst> <net_idx>"},
	{ "key-refresh-set",            NULL,	3,    cmd_key_refresh_set,	"<dst> <net_idx> <phase>"},
	{ "set_start_beacon",	        NULL,	3,    cmd_set_start_beacon,	"<dst> <net_idx> <key>"},
	{ "sethealthtest",	            NULL,	5,    cmd_set_health_test,	"<mode> <dst> <app_idx> <data> <ack>"},
	{ "friend_request",	            NULL,	4,    cmd_friend_request,	"<cache> <offer_delay> <delay> <timeout>"},
	{ "node_mode",		            NULL,	1,    cmd_node_set_mode,		"mode"},
	{ "send_beacon",	            NULL,	2,    cmd_send_beacon,	"<number> <interval>"},
	{ "prov_set_manaual_choose",	NULL,	1,    cmd_prov_set_manaual_choose,	"<enable>"},
	{ "prov_set_choose_par",        NULL,	4,    cmd_prov_set_choose_param,	"<pub_type> <auth> <auth_size> <auth_aciton>"},
	{ "prov_set_static_val",        NULL,	1,    cmd_prov_set_static_val,	"<static value>"},
	{ "prov_set_auth_num",	        NULL,	1,    cmd_prov_set_auth_num,	"<number>"},
	{ "prov_set_pub_key",	        NULL,	1,    cmd_prov_set_pub_key,	"<public key>"},
	{ "prov_set_data",	            NULL,	2,    cmd_prov_set_data,	"<net_idx> <unicast>"},
    { "set_protocol_param",         NULL,   2,    cmd_mesh_set_protocol_param,  "<crpl> <prov_data_resend_interval>"},
    { "iv_testmode",                NULL,   1,    cmd_mesh_set_iv_test_mode,  "<mode>"},
    { "iv_update",                  NULL,   2,    cmd_mesh_set_iv_index_update,  "<iv idex> <flag>"},
    { "iv_get",                     NULL,   0,    cmd_mesh_get_iv_index_update,  ""},
    { "send_vdn_mdl",               NULL,   6,    cmd_vendor_model_data_send,    "<vdn_data> <dst> <dst_ele_idx><netkey_idx><appkey_idx><ttl>"},
	{ NULL, NULL, 0, NULL},
};
