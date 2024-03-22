INCLUDES += -I./include -I$(CONFIG_PREFIX)/usr/include
install:
	-@mkdir -p $(CONFIG_PREFIX)/usr/bin

MFLAGS  += -DMEST_TEST_LOG_ENABLE
MLIBS +=-lell -ljson-c -L$(CONFIG_PREFIX)/usr/lib -lmesh-api
bt_mesh_test: aw_mesh_app.c cmd.c aw_mesh_device_demo.c aw_mesh_vnd_mdl.c
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDES) $(CPPFLAGS) $(MFLAGS) $(MLIBS) $(LDFLAGS)
	@cp bt_mesh_test  $(CONFIG_PREFIX)/usr/bin/

all: bt_mesh_test
clean:
	rm -rf bt_mesh_test
