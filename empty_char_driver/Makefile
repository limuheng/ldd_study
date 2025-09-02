PWD := $(shell pwd)
KERNEL_DIR = /lib/modules/6.6.87.2-microsoft-standard-WSL2+/build

MODULE_NAME = empty_char_driver
obj-m := $(MODULE_NAME).o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
