DRIVER_NAME = lab1_driver
DRIVER2_NAME = lab1_honk_driver

obj-m += $(DRIVER_NAME).o $(DRIVER2_NAME).o $(DRIVER2_NAME)0.o

KDIR = /lib/modules/$(shell uname -r)/build
PWD  = $(shell pwd)
MAKE = /usr/bin/make
KMAKE = $(MAKE) -C $(KDIR) M=$(PWD)

default:
	$(KMAKE) modules

modules_install:
	$(KMAKE) modules_install

insert:
	sudo insmod $(DRIVER_NAME).ko

remove:
	sudo rmmod $(DRIVER_NAME)

clean:
	$(KMAKE) clean

#command definitions
.PHONY: all clean insert remove
