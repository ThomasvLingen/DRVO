DRIVER_NAME = lab2_driver

obj-m += $(DRIVER_NAME).o

KDIR = /lib/modules/$(shell uname -r)/build
PWD  = $(shell pwd)
MAKE = /usr/bin/make
KMAKE = $(MAKE) -C $(KDIR) M=$(PWD)

default:
	$(KMAKE) modules

insert:
	sudo insmod $(DRIVER_NAME).ko
	sudo mknod -m 666 /dev/$(DRIVER_NAME) c 700 0

remove:
	sudo rmmod $(DRIVER_NAME)
	sudo rm -rf /dev/$(DRIVER_NAME)

clean:
	$(KMAKE) clean

#command definitions
.PHONY: all clean insert remove
