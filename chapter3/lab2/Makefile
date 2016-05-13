DRIVER1_NAME = complete_driver
DRIVER2_NAME = init_only_driver
DRIVER3_NAME = exit_only_driver

obj-m += $(DRIVER1_NAME).o $(DRIVER2_NAME).o $(DRIVER3_NAME).o

KDIR = /lib/modules/$(shell uname -r)/build
PWD  = $(shell pwd)
MAKE = /usr/bin/make
KMAKE = $(MAKE) -C $(KDIR) M=$(PWD)

default:
	$(KMAKE) modules

insert:
	sudo insmod $(DRIVER_NAME).ko

remove:
	sudo rmmod $(DRIVER_NAME)

clean:
	$(KMAKE) clean

#command definitions
.PHONY: all clean insert remove
