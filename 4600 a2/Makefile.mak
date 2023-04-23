obj-m += pa2_in.o
obj-m += pa2_out.o
obj-m += shared_buffer.o

KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

input:
	sudo insmod pa2_in.ko
	sudo mknod /dev/pa2_in c $(shell cat /proc/devices | awk '$$2=="pa2_in" {print $$1}') 0
	sudo chmod 666 /dev/pa2_in

output:
	sudo insmod pa2_out.ko
	sudo mknod /dev/pa2_out c $(shell cat /proc/devices | awk '$$2=="pa2_out" {print $$1}') 0
	sudo chmod 666 /dev/pa2_out

test:
	./lkm_pa2-in-test.sh
	./lkm_pa2-out-test.sh

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	sudo rm -f /dev/pa2_in /dev/pa2_out
	sudo rmmod pa2_in pa2_out shared_buffer
