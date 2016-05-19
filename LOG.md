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

## Chapter 4
### Prework: write my own character driver
I actually wrote my own character driver with a read and write operation, since
I thought that'd be more fun than to use the one that is provided.  
The smallest functional character driver I could write seemed to work, with printk printing the following things:  
* `cat /dev/character_driver`:
```
[ 7436.108630] char driver is opening!
[ 7436.108644] char driver READ is not implemented
[ 7436.108648] char driver is closing!
```
* `echo "foo" > /dev/character_driver`:
```
[ 7493.768581] char driver is opening!
[ 7493.768602] char driver WRITE is not implemented
[ 7493.768606] char driver is closing!
```

Note that I also added some Makefile stuff for automatically making and removing a device node, since this was a pain to do by hand. My Makefile now looked like the following:
```makefile
DRIVER_NAME = char_driver

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
```

Next up I wanted to test the driver I made with a program instead of a terminal, so I wrote up a python unittest for this using nosetests. This test does however assume that the driver is loaded and that a device node is instantiated. These can be run with `nosetests`.

### Lab1:
I took the character driver I made, and implemented the behaviour specified in lab 1.
Doing the following:
```
mafn@0xFF /media/mafn/Driver/chapter4/lab1 $ make insert
sudo insmod lab1_driver.ko
sudo mknod -m 666 /dev/lab1_driver c 700 0
mafn@0xFF /media/mafn/Driver/chapter4/lab1 $ cat /dev/lab1_driver
mafn@0xFF /media/mafn/Driver/chapter4/lab1 $ cat /dev/lab1_driver
mafn@0xFF /media/mafn/Driver/chapter4/lab1 $ cat /dev/lab1_driver
```
Yielded this:
```
[ 1192.640413] lab1_driver is initialising
[ 1192.640422] lab1_driver is initialised
[ 1204.942842] lab1_driver is opening [0]!
[ 1204.942848] major/minor: 700/0
[ 1204.942866] lab1_driver READ is not implemented
[ 1204.942875] lab1_driver is closing!
[ 1209.257422] lab1_driver is opening [1]!
[ 1209.257428] major/minor: 700/0
[ 1209.257445] lab1_driver READ is not implemented
[ 1209.257451] lab1_driver is closing!
[ 1210.632327] lab1_driver is opening [2]!
[ 1210.632334] major/minor: 700/0
[ 1210.632348] lab1_driver READ is not implemented
[ 1210.632354] lab1_driver is closing!
```

I also wrote a python test to accompany this.

### Lab2:
This was easy to implement, as all I had to do was move ramdisk allocation from
the init and exit to the open and close respectively.

This was difficult to test since I could not open a file for both reading and
writing at the same time without seek being implemented.

### Lab3:
This was a bit tricky, since it was pretty hard to find out how to actually correctly implement this.
Luckily, I could find some lseek specifications online, and was able to implement it with the following:

```c
static loff_t driver_lseek(struct file* file, loff_t offset, int whence) {
    if (whence == SEEK_SET) {
        pr_info("SETTING POS TO %d\n", offset);
        file->f_pos = offset;
    } else if (whence == SEEK_CUR) {
        pr_info("SETTING POS TO CURRENT + %d\n", offset);
        file->f_pos += offset;
    } else if (whence == SEEK_END) {
        pr_info("SETTING POS TO END + %d\n", offset);
        file->f_pos = file->f_mapping->host->i_size;
    }

    return offset;
}
```

### Lab 4:
I had actually already done this in my makefile, since doing this manually
annoyed me to no end.  
Here is the makefile rule for it:  
```makefile
insert:
	sudo insmod $(DRIVER_NAME).ko
	sudo mknod -m 666 /dev/$(DRIVER_NAME) c 700 0
```

### Lab 5:
To do this, I continued working where the basic character device left off.
This ended up being fairly easy to implement, needing only to add the following:
As a new static variable:
```c
static struct class* driver_class;
```
In the init function:
```c
// Make a device node using udev
driver_class = class_create(THIS_MODULE, "lab5_driver_class");
device_create(driver_class, NULL, major_numbers, "%s", DRIVER_NAME);
```
And in the exit function:
```c
// Remove the device node
device_destroy(driver_class, major_numbers);
class_destroy(driver_class);
```

Now when I `insmod` the driver, the device node is automagically created by udev. Sweet!
