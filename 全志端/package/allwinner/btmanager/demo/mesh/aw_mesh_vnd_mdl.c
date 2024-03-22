#include "cmd.h"
#include <ell/ell.h>
#include "AwmeshSigMdl.h"

#define LOCAL_MODEL AW_APP_MODULE
#define LOG_PRINTF(LEVEL,FMT,...)   mesh_log(LEVEL,LOCAL_MODEL,FMT,##__VA_ARGS__)

#define MAX_VND_NAME_SIZE 32
#define MAX_VND_NAME_MSG_SIZE 36 // OPCODE_SIZE + STATUS + MAX_VND_NAME_SIZE = 3 + 32 + 1
#define XRADIO_COMPANY_ID 0x063D
#define XRADIO_VND_MSG_MDL_ID_SERVER        0x0000
#define XRDIAO_VND_MSG_MDL_ID_CLIENT        0x0001

#define XRADIO_VND_OPCODE_MSG_STAT          0x00
#define XRADIO_VND_OPCODE_MSG_GET           0x01
#define XRADIO_VND_OPCODE_MSG_SET           0x02
#define XRADIO_VND_OPCODE_MSG_SET_UNACK     0x03
#define XRADIO_VND_OPCODE_MAX               0x3F
#define AW_MESH_SIG_VND_OPCODE_MASK         0xC0

#define new0(type, count)   (type *)(malloc(count*sizeof(type)))

typedef struct mesh_vnd_model_info_t
{
    uint8_t element_index;
    uint16_t appkey_idx;
    AW_MESH_MODEL_ID_T model_id;
    struct l_queue *bindings;
}vnd_model_info,*vnd_model_info_p;

static struct mesh_vnd_model_t
{
    struct l_queue *vnd_models;
    uint8_t model_count;
}m_vnd_models;

static bool match_by_model_info(const void *a, const void *b)
{
    if(b == NULL)
        return false;

    if(a == NULL)
        return false;

    if((((vnd_model_info_p)a)->model_id.model_id == ((vnd_model_info_p)b)->model_id.model_id )
        &&(((vnd_model_info_p)a)->element_index == ((vnd_model_info_p)b)->element_index))
    {
        return true;
    }

    return false;
}

static bool match_by_appkey_bind(const void *a, const void *b)
{
    //for test
    return true;
    if(b == NULL || (a!=b))
        return false;

    return true;
}

static bool match_by_app_bind(const void *a, const void *b)
{
    void *appkey_bind = NULL;
    uint32_t appkey;
    if(b == NULL)
        return false;

    if(a == NULL)
        return false;

    if(((vnd_model_info_p)a)->bindings == NULL)
        return false;

    if(((vnd_model_info_p)a)->model_id.model_id != ((vnd_model_info_p)b)->model_id.model_id )
        return false;

    if(((vnd_model_info_p)a)->element_index != ((vnd_model_info_p)b)->element_index)
        return false;

    appkey = ((vnd_model_info_p)b)->appkey_idx;

    //need to check appkey bind.
    appkey_bind = l_queue_find(((vnd_model_info_p)a)->bindings, match_by_appkey_bind, (void *)appkey);

    if(!appkey_bind)
        return false;

    return true;
}

int32_t xradio_vendor_model_receive(AW_MESH_ACCESS_MESSAGE_RX_T *p_msg_rx)
{
    AW_MESH_ACCESS_MESSAGE_TX_T vnd_mdl_msg;
    AW_MESH_ACCESS_PUBLISH_MESSAGE_TX_T vnd_mdl_pub_msg;
    uint16_t mod_id;
    uint16_t company_id = p_msg_rx->opcode.company_id;
    uint16_t opcode = p_msg_rx->opcode.opcode;
    uint8_t *buf = p_msg_rx->data;
    uint8_t len = p_msg_rx->dlen;
    uint8_t tx_msg[MAX_VND_NAME_MSG_SIZE];
    uint8_t tx_len;
    bool msg_tx = false;
    bool msg_pub = false;
    vnd_model_info_p pvnd_model_info = NULL;
    static char vendor_model_name[MAX_VND_NAME_SIZE] = "xradio-mesh";
    vnd_model_info vnd_model = {
        .element_index = p_msg_rx->meta_data.ele_index,
        .appkey_idx = p_msg_rx->meta_data.appkey_index
    };

    if(company_id != XRADIO_COMPANY_ID)
        return AW_ERROR_FAIL;

    if(opcode < AW_MESH_SIG_VND_OPCODE_MASK)
        return AW_ERROR_FAIL;
    opcode &= 0x3F;
    switch(opcode)
    {
        case XRADIO_VND_OPCODE_MSG_STAT:
            mod_id = XRADIO_VND_MSG_MDL_ID_SERVER;
            break;

        case XRADIO_VND_OPCODE_MSG_GET:
        case XRADIO_VND_OPCODE_MSG_SET:
        case XRADIO_VND_OPCODE_MSG_SET_UNACK:
            mod_id = XRDIAO_VND_MSG_MDL_ID_CLIENT;
            break;
        default:
            return AW_ERROR_FAIL;
    }

    vnd_model.model_id.model_id = mod_id;
/*
    pvnd_model_info = l_queue_find(m_vnd_models.vnd_models, match_by_app_bind, (void *)&vnd_model);

    if(pvnd_model_info == NULL)
        return AW_ERROR_FAIL;
*/
    vnd_mdl_msg.meta_data.dst_addr = p_msg_rx->meta_data.src_addr;
    vnd_mdl_msg.meta_data.ele_idx = p_msg_rx->meta_data.src_addr;
    vnd_mdl_msg.meta_data.appkey_index = p_msg_rx->meta_data.appkey_index;

    switch(opcode)
    {
        case XRADIO_VND_OPCODE_MSG_STAT:
            if(len >= 2)
            {
                LOG_PRINTF(AW_DBG_VERB_LEVE,"%s status %d vendor_name %s len %d str = %s\n",__func__,buf[0],&buf[1],len,getstr_hex2str(&buf[1],len-1));
#ifdef MEST_TEST_LOG_ENABLE
                mesh_test_log("%s[rx-meta: src %x dst %x rssi %x dst_ele %x][status %d name %s]",STR_APP_SERVER_XRADIO_VENDOR_NAME_STATUS, \
                    p_msg_rx->meta_data.src_addr,p_msg_rx->meta_data.dst_addr,p_msg_rx->meta_data.rssi,p_msg_rx->meta_data.ele_index,buf[0],&buf[1]);
#endif
            }
            break;

        case XRADIO_VND_OPCODE_MSG_GET:
            LOG_PRINTF(AW_DBG_VERB_LEVE,"%s xradio_vendor_get\n",__func__);
            msg_tx = true;
            opcode = XRADIO_VND_OPCODE_MSG_STAT;
            break;

        case XRADIO_VND_OPCODE_MSG_SET:
            opcode = XRADIO_VND_OPCODE_MSG_STAT;
            msg_tx = true;

        case XRADIO_VND_OPCODE_MSG_SET_UNACK:
            if(len >= 1)
            {
                LOG_PRINTF(AW_DBG_VERB_LEVE,"%s xradio_vendor_name set %s\n",__func__,&buf[0]);
                if(len >= MAX_VND_NAME_SIZE)
                    len = MAX_VND_NAME_SIZE - 1;
                memcpy(&vendor_model_name[0],&buf[0],len);
                vendor_model_name[len] = '\0';
                msg_pub = true;
            }
            break;
        default:
            return AW_ERROR_FAIL;
    }

    if((msg_tx == true)||(msg_pub == true))
    {
        tx_msg[0] = opcode|0xC0;
        tx_msg[1] = XRADIO_COMPANY_ID & 0xFF;
        tx_msg[2] = (XRADIO_COMPANY_ID >> 8) & 0xFF;
        tx_msg[3] = 0;
        tx_len = strlen(vendor_model_name);
        memcpy(&tx_msg[4],&vendor_model_name[0],tx_len + 1);
        tx_msg[tx_len+4] = '\0';
        l_info("src_str %s,dst_str%s\n",&vendor_model_name[0],&tx_msg[4]);
        vnd_mdl_msg.data = &tx_msg[0];
        vnd_mdl_msg.dlen = tx_len + 1 + 4;
        vnd_mdl_msg.meta_data.netkey_index = 0;
        vnd_mdl_msg.meta_data.ttl = 127;
        vnd_mdl_msg.meta_data.ele_idx = p_msg_rx->meta_data.ele_index;
        vnd_mdl_msg.meta_data.appkey_index = p_msg_rx->meta_data.appkey_index;
        vnd_mdl_msg.meta_data.netkey_index = p_msg_rx->meta_data.netkey_index;
        vnd_mdl_msg.meta_data.dst_addr = p_msg_rx->meta_data.src_addr;
        vnd_mdl_msg.meta_data.ele_idx = p_msg_rx->meta_data.ele_index;

        if(msg_tx == true)
            aw_mesh_send_mesh_msg(&vnd_mdl_msg);

        if(msg_pub == true)
        {
            vnd_mdl_pub_msg.data = vnd_mdl_msg.data;
            vnd_mdl_pub_msg.dlen = vnd_mdl_msg.dlen;
            memcpy(&vnd_mdl_pub_msg.meta_data,&vnd_mdl_pub_msg.meta_data,sizeof(AW_MESH_ACCESS_TX_META_T));
            vnd_mdl_pub_msg.mod_id = (XRADIO_COMPANY_ID <<16)|XRADIO_VND_MSG_MDL_ID_SERVER;
            aw_mesh_publish_vendor_mesh_msg(&vnd_mdl_pub_msg);
        }
    }
    return AW_ERROR_NONE;
}

int32_t vendor_model_reg(uint8_t element_index,uint16_t model_id)
{
    struct l_queue *vnd_models = m_vnd_models.vnd_models;
    struct l_queue *vnd_binds = NULL;

    vnd_model_info_p pvnd_model_new = NULL;
    uint32_t vnd_model_id,status;

    vnd_model_info vnd_model_reg = {
        .element_index = element_index,
        .model_id = model_id
    };

    if(vnd_models == NULL)
    {
        if((vnd_models = l_queue_new())==NULL)
        {
            LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error, new vnd models queue fail!\n");
            return AW_ERROR_FAIL;
        }
        m_vnd_models.vnd_models = vnd_models;
    }

    pvnd_model_new = l_queue_find(vnd_models, match_by_model_info, (void *)&vnd_model_reg);

    if(pvnd_model_new)
    {
        return AW_ERROR_NONE;
    }

    pvnd_model_new = new0(vnd_model_info, 1);
    vnd_binds = l_queue_new();
    if((!pvnd_model_new)||(!vnd_binds))
    {
        if(!pvnd_model_new)
            free(pvnd_model_new);

        if(vnd_binds)
            l_queue_destroy(vnd_binds, NULL);

        return AW_ERROR_FAIL;
    }

    pvnd_model_new->element_index = element_index;
    pvnd_model_new->model_id.model_id = model_id;
    pvnd_model_new->bindings = vnd_binds;

    vnd_model_id = XRADIO_COMPANY_ID << 16 | model_id;

    status = aw_mesh_add_model(element_index,vnd_model_id,(void*)pvnd_model_new);

    if (!l_queue_push_tail(m_vnd_models.vnd_models, pvnd_model_new)) {
        free(pvnd_model_new);
        return AW_ERROR_FAIL;
    }

    LOG_PRINTF(AW_DBG_VERB_LEVE,"mesh vnd model reg and current count %d\n",l_queue_length(m_vnd_models.vnd_models));

    return status;
}
