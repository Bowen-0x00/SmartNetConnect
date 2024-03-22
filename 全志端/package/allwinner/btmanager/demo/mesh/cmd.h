#include "AWTypes.h"

typedef struct {
	const char *cmd;
	const char *arg;
    UINT32 arg_cnt;
	void (*func) (int argc, char *args[]);
	const char *desc;
} cmd_tbl_t;
extern int32_t __main_terminated;
extern cmd_tbl_t cmd_table[];
void aw_mesh_demo_device_init();
void aw_mesh_demo_goo_send(uint16_t dst,uint16_t appkey_idx,uint16_t on_off);
int32_t vendor_model_reg(uint8_t element_index,uint16_t model_id);
