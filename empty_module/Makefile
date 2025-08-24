PWD := $(shell pwd)
KERNEL_DIR = /workspace/buildroot/output/build/linux-6.12.27

MODULE_NAME = empty_module
# obj-y: build static linked module
# obj-n: not build
# obj-m: build dynamic linked module
obj-m := $(MODULE_NAME).o

all:
	make ARCH=arm CROSS_COMPILE=arm-buildroot-linux-gnueabihf- \
		-C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean