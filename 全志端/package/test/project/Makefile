CTOOL:=riscv64-unknown-linux-gnu-
CCL:=/home/bowen/OSHW/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/
CC:=${CCL}/bin/${CTOOL}gcc
CFLAGS := 

objects := wiringPi.o wiringSerial.o pn532.o pn532_nezha.o libwifimg.so
# sunxi_gpio.o pinmux.o
# random.o  random.o
#out/,*d1s*,**d1-s**,*nezha_min*,*arm*,*mips*,*/arc/,*nds32*,*nios2*,*/x86/*,*/xtensa/*,*/sandbox/*,*/c6x/*,*/h8300/*,*/microblaze/*,*/openrisc/*,*/powerpc/*,*.md,*.txt,README*,*/m68k*,*arch/sparc*,*Documentation*,*samsung*
all:dump writeInfo readInfo ConnectWifiByNFC
	
dump: $(objects) test.o
	${CC} ${CFLAGS} -o  dump $(objects) test.o

	
writeInfo: $(objects) writeInfo.o
	${CC} ${CFLAGS} -o writeInfo $(objects) writeInfo.o

readInfo: $(objects) readInfo.o
	${CC} ${CFLAGS} -o readInfo $(objects) readInfo.o

ConnectWifiByNFC: $(objects) ConnectWifiByNFC.o
	${CC} ${CFLAGS} -o ConnectWifiByNFC $(objects) ConnectWifiByNFC.o


# PATH := /home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/btmanager/demo/

# #lbtmg
# CFLAGS := -Os -pipe -mcmodel=medany -mabi=lp64d -march=rv64gcxthead -g3 -fno-caller-saves -Wno-format-truncation -Wno-unused-result  -Wformat -Werror=format-security -Wl,-z,now -Wl,-z,relro
# INCLUDES := -I$(PATH)/include -I/home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/btmanager/ipkg-install/usr/include
# LIBS := -L/home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/btmanager/ipkg-install/usr/lib -lbluetooth -lglib-2.0 -lsbc -ljson-c -lgio-2.0 -lgobject-2.0 -lasound -lbtmg -ldl -lm -lresolv -lshared-mainloop -lbluetooth-internal
# LDFLAGS := -L/home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/lib -L/home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/staging_dir/target/lib -L/home/bowen/OSHW/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/usr/lib -L/home/bowen/OSHW/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/lib -znow -zrelro -lpthread -lz -lrt -lm -ldl


# LIBS := -L/home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/btmanager/ipkg-install/usr/lib -lbluetooth -lglib-2.0 -lsbc -ljson-c -lgio-2.0 -lgobject-2.0 -lasound -lbtmg -ldl -lm -lresolv -lshared-mainloop -lbluetooth-internal
# LDFLAGS := -L/home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/lib -L/home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/staging_dir/target/lib -L/home/bowen/OSHW/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/usr/lib -L/home/bowen/OSHW/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/lib -znow -zrelro -lpthread -lz -lrt -lm -ldl
# SO := /home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/glib-2.50.1/gmodule/.libs/libgmodule-2.0.so
# "-L/home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/staging_dir/target/usr/lib -L/home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/staging_dir/target/lib -L/home/bowen/OSHW/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/usr/lib -L/home/bowen/OSHW/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/lib -znow -zrelro -lpthread -lz -lrt -lm -ldl"
# CONFIG_PREFIX="/home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/btmanager/ipkg-install"
# /home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/
# LIBS += -L$(LIB_PATH) -lbluetooth -lglib-2.0 -lsbc -ljson-c -lgio-2.0 -lgobject-2.0 -lasound -lbtmg -ldl -lm -lresolv
# LIBS+=-lshared-mainloop -lbluetooth-internal
# LIBS := -L$(LIB_PATH) -lbluetooth -lglib-2.0 -lsbc  -lgio-2.0    -ldl -lm -lresolv

# LIB_PATH := /home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/bluez-5.54/lib/.libs/
# LIBS+= -L$(LIB_PATH) -lshared-mainloop 
# LIB_PATH := /home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/bluez-5.54/src/.libs/
# LIBS+= -L$(LIB_PATH) -lbluetooth-internal
# LIB_PATH := /home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/rootfs/usr/lib/
# LIBS+= -L$(LIB_PATH) -lgobject-2.0
# LIB_PATH := /home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/alsa-lib-1.1.4.1/src/.libs/
# LIBS+= -L$(LIB_PATH) -lasound
# LIB_PATH := /home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/json-c-0.13.1/.libs/
# LIBS+= -L$(LIB_PATH) -ljson-c
# LIB_PATH := /home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/rootfs/lib
# LIBS+= -L$(LIB_PATH) -lbtmg

# LIB_PATH := /home/bowen/OSHW/tina-d1-h/out/d1-h-nezha/compile_dir/target/bluez-5.54/lib/.libs/
# LIBS+= -L$(LIB_PATH) -lbtmg

# C_SRCS = $(PATH)bt_cmd.c $(PATH)bt_gatt_server_app.c $(PATH)bt_gatt_client_app.c ConnectWifiByBT.c
# C_OBJS := $(C_SRCS:.c=.o)
# LIB_OBJS = $(C_OBJS)

# ConnectWifiByBT: $(LIB_OBJS)
# 	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDES) -g $(LIBS)  $(LDFLAGS)
# ConnectWifiByBT: libwifimg.so ConnectWifiByBT.o bt_gatt_server_app.o $(SO)
# 	${CC} -o ConnectWifiByBT libwifimg.so ConnectWifiByBT.o bt_gatt_server_app.o $(SO) $(CFLAGS) -g $(LIBS) $(LDFLAGS)

# test.o:test.c
# cd /tmp && you-operation-cmds
# test.o:test.c
# 	${CC} -c test.c

# pn532.o:pn532.c
# 	${CC} -c pn532.c

# wiringPi.o:wiringPi.c
# 	${CC} -c wiringPi.c

# wiringSerial.o:wiringSerial.c
# 	${CC} -c wiringSerial.c

# pn532_nezha.o:pn532_nezha.c
# 	${CC} -c pn532_nezha.c
	
.PHONY: clean	
clean:
	-rm *.o