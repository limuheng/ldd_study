PWD := $(shell pwd)
KERNEL_DIR = /lib/modules/6.6.87.2-microsoft-standard-WSL2+/build

CFILES := simple_char_driver_impl.c simple_char_driver.c

MODULE_NAME = scd_module
obj-m := $(MODULE_NAME).o

$(MODULE_NAME)-objs := $(CFILES:.c=.o)

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
