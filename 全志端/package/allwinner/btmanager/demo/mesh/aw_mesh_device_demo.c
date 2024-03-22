#include "cmd.h"
#include <ell/ell.h>
#include "AwmeshSigMdl.h"

#define LOCAL_MODEL AW_APP_MODULE
#define LOG_PRINTF(LEVEL,FMT,...)   mesh_log(LEVEL,LOCAL_MODEL,FMT,##__VA_ARGS__)

#define new0(type, count)   (type *)(malloc(count*sizeof(type)))

int32_t xradio_vendor_model_receive(AW_MESH_ACCESS_MESSAGE_RX_T *p_msg_rx);
int32_t vendor_model_reg(uint8_t element_index,uint16_t model_id);

static struct mesh_device_t
{
    struct l_queue *elements;
    uint16_t dst;
}m_demo_device;

struct mesh_element_t
{
    uint16_t ele_idx;
    struct mesh_device_t *pm_device;
    struct l_queue *models;
};

struct mesh_model_t
{
    uint32_t model_id;
    void *db;
    struct mesh_element_t *pm_element;
};

struct mesh_model_goo_server_db_t
{
     struct mesh_model_t *pm_model;
     bool present_onoff;
     bool target_onoff;
     AW_MESH_MDL_TRANS_TIME_T trans_time;
     AW_MESH_MDL_TRANS_TIME_T total_time;
     AW_MESH_MDL_TRANS_TIME_T remaining_time;
     AW_MESH_SIG_MDL_META_T meta;
};

struct mesh_model_goo_client_db_t
{
     struct mesh_model_t *pm_model;
     AW_MESH_SIG_MDL_META_T meta;
};

static void on_off_bindings_updata()
{
    //need to realize
}

static void on_off_value_publish(struct mesh_model_goo_server_db_t *db)
{
    AW_MESH_SIG_MDL_MSG_TX_T mdl_tx_msg;
    //fill meta data
    mdl_tx_msg.meta.model_id = db->meta.model_id;
    mdl_tx_msg.meta.src_addr = db->meta.src_addr;
    mdl_tx_msg.meta.src_element_index = db->meta.src_element_index;
    mdl_tx_msg.meta.ttl = db->meta.ttl;
    mdl_tx_msg.meta.msg_appkey_index = db->meta.msg_appkey_index;
    mdl_tx_msg.meta.msg_netkey_index = db->meta.msg_netkey_index;
    //fill on off status
    mdl_tx_msg.opcode = AW_MESH_MSG_GENERIC_ONOFF_STATUS;
    mdl_tx_msg.data.goo_status.b_target_present = false;
    mdl_tx_msg.data.goo_status.present_onoff = db->present_onoff;

    aw_mesh_send_sig_model_msg(&mdl_tx_msg);
}

static void on_off_value_set(struct mesh_model_goo_server_db_t *db, bool on_off)
{
    if(db->present_onoff != on_off)
    {
        db->present_onoff = on_off;
#if(ENABLE_MODEL_BINDINGS == 1)
        on_off_bindings_updata();
#endif
        on_off_value_publish(db);
    }
#ifdef MEST_TEST_LOG_ENABLE
        mesh_test_log("%s[onoff %x]",STR_APP_SERVER_GOO_SET,on_off);
#endif
}

static void demo_model_goo_server_msg_receive(AW_MESH_SIG_MDL_MSG_RX_T *mdl_msg_rx, struct mesh_model_t *p_model)
{
    uint16_t opcode = mdl_msg_rx->opcode;

    struct mesh_model_goo_server_db_t *p_goo_server_db = (struct mesh_model_goo_server_db_t *)p_model->db;

    switch(opcode)
    {
        case AW_MESH_MSG_GENERIC_DTT_GET:
            LOG_PRINTF(AW_DBG_VERB_LEVE,"trans_time get num_steps %x step_resolution %x",p_goo_server_db->trans_time.num_steps,p_goo_server_db->trans_time.step_resolution);
            mdl_msg_rx->data.gdtt_status.trans_time = p_goo_server_db->trans_time;
            break;

        case AW_MESH_MSG_GENERIC_ONOFF_GET:
            mdl_msg_rx->data.goo_get.present_onoff = p_goo_server_db->present_onoff;
            LOG_PRINTF(AW_DBG_VERB_LEVE,"goo server get onoff %d",p_goo_server_db->present_onoff);
#ifdef MEST_TEST_LOG_ENABLE
            mesh_test_log("%s[rx-meta: src %x dst %x rssi %x dst_ele %x][onoff-get %x]",STR_APP_SERVER_GOO_OP, \
                mdl_msg_rx->meta.src_addr,mdl_msg_rx->meta.dst_addr,mdl_msg_rx->meta.rssi,mdl_msg_rx->meta.src_element_index,  \
                    p_goo_server_db->present_onoff);
#endif

            break;

        case AW_MESH_MSG_GENERIC_ONOFF_SET:
        case AW_MESH_MSG_GENERIC_ONOFF_SET_UNACK:
            p_goo_server_db->total_time = mdl_msg_rx->data.goo_set.total_time;
            p_goo_server_db->remaining_time = mdl_msg_rx->data.goo_set.remaining_time;
            if(mdl_msg_rx->data.goo_set.remaining_time.num_steps == 0)
            {
                on_off_value_set(p_goo_server_db,mdl_msg_rx->data.goo_set.target_onoff);
            }
            LOG_PRINTF(AW_DBG_VERB_LEVE,"goo server set target onoff %d and present onoff %d",  \
                mdl_msg_rx->data.goo_set.target_onoff,p_goo_server_db->present_onoff);
#ifdef MEST_TEST_LOG_ENABLE
            if(AW_MESH_MSG_GENERIC_ONOFF_SET == opcode)
                mesh_test_log("%s[rx-meta: src %x dst %x rssi %x dst_ele %x][onoff-set %x resolution %x steps %x]",STR_APP_SERVER_GOO_OP,   \
                    mdl_msg_rx->meta.src_addr,mdl_msg_rx->meta.dst_addr,mdl_msg_rx->meta.rssi,mdl_msg_rx->meta.src_element_index,  \
                        mdl_msg_rx->data.goo_set.target_onoff, mdl_msg_rx->data.goo_set.remaining_time.step_resolution,mdl_msg_rx->data.goo_set.remaining_time.num_steps);
            else
                mesh_test_log("%s[rx-meta: src %x dst %x rssi %x dst_ele %x][onoff-set-unack %x resolution %x steps %x]",STR_APP_SERVER_GOO_OP, \
                    mdl_msg_rx->meta.src_addr,mdl_msg_rx->meta.dst_addr,mdl_msg_rx->meta.rssi,mdl_msg_rx->meta.src_element_index,  \
                        mdl_msg_rx->data.goo_set.target_onoff,mdl_msg_rx->data.goo_set.remaining_time.step_resolution,mdl_msg_rx->data.goo_set.remaining_time.num_steps);
#endif
            break;

        default:
            LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,unknow opcode %x \n",__func__,opcode);
            break;
    }
}

static void demo_model_goo_client_msg_receive(AW_MESH_SIG_MDL_MSG_RX_T *mdl_msg_rx, struct mesh_model_t *p_model)
{
    struct mesh_model_goo_client_db_t *p_goo_client_db = (struct mesh_model_goo_client_db_t *)p_model->db;
    uint16_t opcode = mdl_msg_rx->opcode;
    AW_MESH_GOO_STATUS_T *p_status;
    switch(opcode)
    {
        case AW_MESH_MSG_GENERIC_ONOFF_STATUS:
            p_status = &mdl_msg_rx->data.goo_status;
            if(p_status->b_target_present)
            {
                LOG_PRINTF(AW_DBG_VERB_LEVE,"goo client receive: present = %d, target = %d, remain time = step(%d), resolution(%d)\r\n",    \
                    p_status->present_onoff, p_status->target_onoff,p_status->remaining_time.num_steps, p_status->remaining_time.step_resolution);
#ifdef MEST_TEST_LOG_ENABLE
                mesh_test_log("%s[rx-meta: src %x dst %x rssi %x dst_ele %x][present-onoff %x target-onoff %x remain time %x step %x resolution %x]",STR_APP_CLIENT_GOO_STATUS, \
                    mdl_msg_rx->meta.src_addr,mdl_msg_rx->meta.dst_addr,mdl_msg_rx->meta.rssi,mdl_msg_rx->meta.src_element_index,   \
                        p_status->present_onoff, p_status->target_onoff,p_status->remaining_time.num_steps, p_status->remaining_time.step_resolution);
#endif
            }
            else
            {
                LOG_PRINTF(AW_DBG_VERB_LEVE,"goo client receive: present = %d\r\n", p_status->present_onoff);
#ifdef MEST_TEST_LOG_ENABLE
                mesh_test_log("%s onoff %x [rx-meta: src %x dst %x rssi %x dst_ele %x]",STR_APP_CLIENT_GOO_STATUS,p_status->present_onoff,mdl_msg_rx->meta.src_addr,mdl_msg_rx->meta.dst_addr,mdl_msg_rx->meta.rssi,mdl_msg_rx->meta.src_element_index);
#endif
            }
            break;

        default:
            LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,unknow opcode %x \n",__func__,opcode);
            break;
    }
}

void demo_vnd_model_msg_receive(AW_MESH_EVENT_T *event,void *private)
{
    AW_MESH_ACCESS_MESSAGE_RX_T *p_msg_rx;
    int32_t status;

    if(event == NULL)
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,event %p private %p \n",__func__,event,private);
        return ;
    }
    if(event->evt_code != AW_MESH_EVENT_ACCESS_MSG)
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,event->evt_code %d \n",__func__,event->evt_code);
        return ;
    }


    p_msg_rx = &event->param.access_rx_msg;
    LOG_PRINTF(AW_DBG_VERB_LEVE,"%s cid %x opcode %x len %d data %s\n",__func__,p_msg_rx->opcode.company_id,p_msg_rx->opcode.opcode,p_msg_rx->dlen,getstr_hex2str(p_msg_rx->data,p_msg_rx->dlen));

    if(p_msg_rx->opcode.company_id == 0x063D)
    {
        status = xradio_vendor_model_receive(p_msg_rx);
        if(status == AW_ERROR_NONE)
        {
            LOG_PRINTF(AW_DBG_VERB_LEVE,"%s succeed ,src %d to dst %x ,opcode %x , appkey_idx %x , rssi %d  \n",__func__,p_msg_rx->meta_data.src_addr,    \
                p_msg_rx->meta_data.dst_addr,p_msg_rx->opcode.opcode,p_msg_rx->meta_data.appkey_index,(INT8)p_msg_rx->meta_data.rssi);
        }
        else
        {
            LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,status %d,cid %x ,opcode %x , appkey_idx %x \n",__func__,status,p_msg_rx->opcode.company_id,p_msg_rx->opcode.opcode,p_msg_rx->meta_data.appkey_index);
        }
    }
}

void demo_model_msg_receive(AW_MESH_EVENT_T *event,void *private)
{
    struct mesh_model_t *p_model = private;
    if(event == NULL)
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,event %p private %p \n",__func__,event,private);
        return ;
    }

    if(event->evt_code != AW_MESH_EVENT_MODEL_RX_CB)
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,event->evt_code %d \n",__func__,event->evt_code);
        return ;
    }

    switch(p_model->model_id)
    {
        case AW_MESH_SIG_MODEL_ID_GENERIC_ONOFF_SERVER:
            demo_model_goo_server_msg_receive(&event->param.model_rx_msg,p_model);
            break;
        case AW_MESH_SIG_MODEL_ID_GENERIC_ONOFF_CLIENT:
            demo_model_goo_client_msg_receive(&event->param.model_rx_msg,p_model);
            break;
        default:
            LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,demo model_id 0x%x not support\n",__func__,p_model->model_id);
            break;
    }
}

//new goo client model and goo servel model
static void demo_new_goo_model(struct mesh_element_t *p_element)
{
    struct mesh_model_t *p_model_goo_server,*p_model_goo_client;
    AW_MESH_SIG_MDL_REG_T mdl_reg_goo_server,mdl_reg_goo_client;
    struct mesh_model_goo_server_db_t *goo_server_db;
    struct mesh_model_goo_client_db_t *goo_client_db;

    if((p_element == NULL)||(p_element->models == NULL))
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,p_element %p p_element->models %p \n",__func__,p_element,p_element->models);
        return ;
    }

    //create goo server model
    p_model_goo_server = new0(struct mesh_model_t,1);

    if(p_model_goo_server == NULL)
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,new model fail!\n");
        return ;
    }
    //create goo server model database
    goo_server_db = new0(struct mesh_model_goo_server_db_t,1);

    if(goo_server_db == NULL)
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,new server database fail!\n");
        free(goo_server_db);
        free(p_model_goo_server);
        return ;
    }
    memset(goo_server_db,0,sizeof(struct mesh_model_goo_server_db_t));
    memset(p_model_goo_server,0,sizeof(struct mesh_model_t));
    p_model_goo_server->pm_element = p_element;
    p_model_goo_server->model_id = AW_MESH_SIG_MODEL_ID_GENERIC_ONOFF_SERVER;
    //register goo server model
    mdl_reg_goo_server.ele_idx      = p_element->ele_idx;
    mdl_reg_goo_server.model_id     = p_model_goo_server->model_id;
    mdl_reg_goo_server.app_private  = p_model_goo_server;
    goo_server_db->meta.model_id = mdl_reg_goo_server.model_id;
    aw_mesh_register_sig_model(&mdl_reg_goo_server);
    //push server model into models queue of element
    p_model_goo_server->db = goo_server_db;
    l_queue_push_tail(p_element->models,p_model_goo_server);

    //create goo client model
    p_model_goo_client = new0(struct mesh_model_t,1);

    if(p_model_goo_client == NULL)
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,new model fail!\n");
        return ;
    }
    //create goo client  model database
    goo_client_db = new0(struct mesh_model_goo_client_db_t,1);

    if(goo_client_db == NULL)
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,new server database fail!\n");
        free(goo_client_db);
        free(p_model_goo_client);
        return ;
    }
    memset(goo_client_db,0,sizeof(struct mesh_model_goo_client_db_t));
    memset(p_model_goo_client,0,sizeof(struct mesh_model_t));
    p_model_goo_client->pm_element = p_element;
    p_model_goo_client->model_id = AW_MESH_SIG_MODEL_ID_GENERIC_ONOFF_CLIENT;
    //register goo client model
    mdl_reg_goo_client.ele_idx      = p_element->ele_idx;
    mdl_reg_goo_client.model_id     = p_model_goo_client->model_id;
    mdl_reg_goo_client.app_private  = p_model_goo_client;
    goo_client_db->meta.model_id = mdl_reg_goo_client.model_id;
    aw_mesh_register_sig_model(&mdl_reg_goo_client);
    //push server model into models queue of element
    p_model_goo_client->db = goo_client_db;
    l_queue_push_tail(p_element->models,p_model_goo_client);

}

static void demo_new_vendor_model(uint16_t ele_idx)
{
    vendor_model_reg(ele_idx,0);
    vendor_model_reg(ele_idx,1);
}

//new element with goo client model and goo server model .

static void demo_new_goo_element(struct mesh_device_t *p_device)
{
    struct mesh_element_t *p_element;

    if(p_device == NULL)
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error, device is NULL\n");
        return ;
    }

    if(p_device->elements == NULL)
    {
        if((p_device->elements = l_queue_new())==NULL)
        {
            LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error, new element queue fail!\n");
            return ;
        }
    }

    p_element = new0(struct mesh_element_t,1);

    if(p_element == NULL)
    {
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,new element fail!\n");
        return ;
    }

    memset(p_element,0,sizeof(struct mesh_element_t));
    p_element->pm_device = p_device;

    if((p_element->models = l_queue_new())== NULL)
    {
        free(p_element);
        LOG_PRINTF(AW_DBG_VERB_LEVE,"%s error ,new model queue fail!\n");
        return ;
    }

    aw_mesh_add_element(0,&p_element->ele_idx);

    demo_new_goo_model(p_element);
    demo_new_vendor_model(p_element->ele_idx);
}

void aw_mesh_demo_device_init()
{
    memset(&m_demo_device,0,sizeof(struct mesh_device_t));
    demo_new_goo_element(&m_demo_device);
}

void aw_mesh_demo_goo_send(uint16_t dst,uint16_t appkey_idx,uint16_t on_off)
{
    AW_MESH_SIG_MDL_MSG_TX_T sigMdl_tx_msg;
}
