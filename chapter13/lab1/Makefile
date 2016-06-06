DRIVER_NAME = lab1_driver

obj-m += $(DRIVER_NAME).o

KDIR = /lib/modules/$(shell uname -r)/build
PWD  = $(shell pwd)
MAKE = /usr/bin/make
KMAKE = $(MAKE) -C $(KDIR) M=$(PWD)

USERSPACE_SRC = lab1_userspace.c
USERSPACE_OUT = lab1_userspace

default:
	$(KMAKE) modules

userspace_program:
	gcc $(USERSPACE_SRC) -o $(USERSPACE_OUT)

insert:
	sudo insmod $(DRIVER_NAME).ko

remove:
	sudo rmmod $(DRIVER_NAME)

clean:
	$(KMAKE) clean
	rm -rf $(USERSPACE_OUT)

#command definitions
.PHONY: all clean insert remove userspace_program
