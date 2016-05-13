## Preparation work VM
* Download latest Linux Mint ISO
* Set up shared folders for sources (mount in rc.local since I could not get fstab to work)
* Shared folder was owned by root, modified rc.local to include `mount.vboxsf -o uid=1000,gid=1000 DriverOntwikkeling /media/mafn/Driver vboxsf
exit `
* sudo apt-get install vim gparted g++
* add my custom aliases to .bashrc
```
alias lss='ls -A'
alias mafn_drivers='cd /media/mafn/Driver'
alias mafn_kernel_msg='watch "dmesg | tail -25"'
```


## Chapter 2 (LAB 2.8)
* Download sample makefile and edit it to my liking

Next I made a sample hello_world driver which printks hello and goodbye when
it inits and exits respectively. Next up I built it with my Makefile, if this
sample driver succeeds, I can confirm that my VM setup is working and good to go.

The results were exciting, building the hello_world_driver succeeded on first
try, and by doing `insmod hello_world_driver.ko` and `rmmod hello_world_driver.ko`
resulted in the following output from `dmesg | tail`:

```
[ 4618.842029] Hello world driver is initialising
[ 4633.710124] Hello world driver is exitting
```

The full source for the `hello_world kernel driver`:
```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init some_init(void) {
    printk("Hello world driver is initialising\n");
    return 0;
}

static void __exit some_exit(void) {
    printk("Hello world driver is exitting\n");
}

module_init(some_init);
module_exit(some_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mafn");
MODULE_DESCRIPTION("Minimal hello world driver");
MODULE_VERSION("printk");
```

The full source for the `Makefile`:
```makefile
DRIVER_NAME = hello_world_driver

obj-m += $(DRIVER_NAME).o

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
```

## Chapter 3
### Lab 1
Next up was writing a driver which takes an integer parameter. This seemed to work. Doing the following:
```
mafn@0xFF /media/mafn/Driver/chapter3 $ sudo insmod counter_driver.ko param_int=10
mafn@0xFF /media/mafn/Driver/chapter3 $ make remove
sudo rmmod counter_driver
mafn@0xFF /media/mafn/Driver/chapter3 $
```

Yielded this as output:
```
[ 7398.353619] Int parameter driver is initialising
[ 7398.353623] Value of the parameter during init = 10
[ 7518.907365] Value of the parameter during exit = 10
[ 7518.907369] Int parameter driver is exitting
```

I however couldn't find the parameter in /sys/modules/<driver_name>  
Reason: didn't pass the right permission options in the `module_param` call

And thus I modified `module_param(param_int, int, 0);` to become `module_param(param_int, int, 0755);`  

Now, I could actually see the module param in the correct folder and read it with `cat`.

### Lab 2
For lab 2 I made 3 separate drivers:
* complete_driver (init & exit)
* init_only_driver (init)
* exit_only_driver (exit)

The behaviour for these drivers is as follows:  
**complete_driver**:  
Behaviour is as intended, the driver can be loaded and unloaded as per usual.  
**init_only_driver**:  
This driver can be loaded, but no unloaded. See the following output
```
mafn@0xFF /media/mafn/Driver/chapter3/lab2 $ sudo insmod init_only_driver.ko
[  858.848913] init only driver is initialising
mafn@0xFF /media/mafn/Driver/chapter3/lab2 $ sudo rmmod init_only_driver.ko
rmmod: ERROR: ../libkmod/libkmod-module.c:769 kmod_module_remove_module() could not remove 'init_only_driver': Device or resource busy
rmmod: ERROR: could not remove module init_only_driver.ko: Device or resource busy
```  
**exit_only_driver**:  
This driver can also be loaded and unloaded as per usual.
