CC = arm-linux-gnueabihf-gcc
LD = arm-linux-gnueabihf-ld
AR = arm-linux-gnueabihf-ar
OBJCOPY = arm-linux-gnueabihf-objcopy
OBJDUMP = arm-linux-gnueabihf-objdump

CFLAGS := -Wall -O2 -g -nostdlib
INCLUDES = -I ./include

objs := start.o main.o led.o gpio.o clock_init.o uart.o gic.o eabi_compat.o

TARGET :=led

${TARGET}:${objs}
	${LD} -T imx6ull.lds -o $@_elf $^ 
	${OBJCOPY} -O binary -S $@_elf $@.bin
	${OBJDUMP} -D -m arm $@_elf > $@.dis
	./tools/mkimage -n ./tools/imximage.cfg.cfgtmp -T imximage -e 0x80100000 -d $@.bin $@.imx
	dd if=/dev/zero of=1k.bin bs=1024 count=1
	cat 1k.bin $@.imx > $@.img

%.o:%.c
	${CC} ${CFLAGS} ${INCLUDES} -c -o $@ $<	

%.o:%.S
	${CC} ${CFLAGS} ${INCLUDES} -c -o $@ $<

clean:
	rm -f ${TARGET}.bin ${TARGET}_elf ${TARGET}.dis ${TARGET}.img *.o ${TARGET}.imx
