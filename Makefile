CC = arm-linux-gnueabihf-gcc
LD = arm-linux-gnueabihf-ld
AR = arm-linux-gnueabihf-ar
OBJCOPY = arm-linux-gnueabihf-objcopy
OBJDUMP = arm-linux-gnueabihf-objdump
CFLAGS =  -Wall -O2 -g -nostdlib -fno-builtin
INCLUDES = -I ./include -nostdinc
LDFLAGS	= -lgcc -L /home/drew/work/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/6.2.1

objs := start.o main.o led.o gpio.o clock_init.o uart.o gic.o key_interrupt.o i2c.o my_printf.o string_utils.o

TARGET := led

${TARGET}.elf:${objs}
	${LD} -T imx6ull.lds -g -o $@ $^ ${LDFLAGS}
	${OBJCOPY} -O binary -S $@ ${TARGET}.bin
	${OBJDUMP} -D -m arm $@ > ${TARGET}.dis
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d ${TARGET}.bin ${TARGET}.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin ${TARGET}.imx > ${TARGET}.img

%.o:%.c
	${CC} ${CFLAGS} ${INCLUDES} -c -o $@ $<	

%.o:%.S
	${CC} ${CFLAGS} ${INCLUDES} -c -o $@ $<

clean:
	rm -f *.bin *.elf *.dis *.img *.o *.imx


