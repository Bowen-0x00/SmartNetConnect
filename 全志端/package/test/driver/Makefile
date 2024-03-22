CTOOL:=riscv64-unknown-linux-gnu-
CCL:=/home/bowen/OSHW/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/
CC:=${CCL}/bin/${CTOOL}gcc

obj-m := key.o

KDIR := /home/bowen/OSHW/tina-d1-h/lichee/linux-5.4/

all:
	make ARCH=riscv CROSS_COMPILE=${CCL}/bin/${CTOOL} -C $(KDIR) M=$(PWD) modules

clean:
    make -C $(KDIR) M=$(PWD) clean