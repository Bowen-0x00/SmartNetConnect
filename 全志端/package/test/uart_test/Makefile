CTOOL:=riscv64-unknown-linux-gnu-
CCL:=/home/bowen/OSHW/tina-d1-h/prebuilt/gcc/linux-x86/riscv/toolchain-thead-glibc/riscv64-glibc-gcc-thead_20200702/
CC:=${CCL}/bin/${CTOOL}gcc


uart_test:uart_test.c
	${CC} -o uart_test uart_test.c


clean:
	rm uart_test