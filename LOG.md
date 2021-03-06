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

## Chapter 6
### Lab 1
`strace ls 2>&1 | less`  
I was surprised by how many system calls a simple `ls` of a directory makes!

## Chapter 8
### Lab 1
The goal here is to make a simple driver which shares its IRQ with my network
card. So the first step was figuring out which interrupt number corresponds to
some sort of network connection. doing `cat /proc/devices` revealed the following:
```
CPU0       
0:         36    XT-PIC  timer
1:       1767    XT-PIC  i8042
2:          0    XT-PIC  cascade
8:          0    XT-PIC  rtc0
9:      10497    XT-PIC  acpi, vboxguest
10:      12108    XT-PIC  eth0
11:      29573    XT-PIC  ohci_hcd:usb1, 0000:00:0d.0, snd_intel8x0
12:       3416    XT-PIC  i8042
14:          0    XT-PIC  ata_piix
15:       7005    XT-PIC  ata_piix
NMI:          0   Non-maskable interrupts
LOC:    1527532   Local timer interrupts
SPU:          0   Spurious interrupts
PMI:          0   Performance monitoring interrupts
IWI:          2   IRQ work interrupts
RTR:          0   APIC ICR read retries
RES:          0   Rescheduling interrupts
CAL:          0   Function call interrupts
TLB:          0   TLB shootdowns
TRM:          0   Thermal event interrupts
THR:          0   Threshold APIC interrupts
MCE:          0   Machine check exceptions
MCP:         24   Machine check polls
HYP:          0   Hypervisor callback interrupts
ERR:          0
MIS:          0
```

It was not difficult to work out that eth0 (ethernet) had something to do with
my network connection. An interrupt seemed to be generated once every 2 seconds.
But with stimulating using `ping 8.8.8.8` far more were generated.

Next up was writing a driver which made an interrupt handler, registered it for
the eth0 interrupt and counts how many times it's called. The result can be found
in `chapter8/lab1`.

For some reason I couldn't register the interrupt handler. I fiddled around a bit
and this was apparently because I used `NULL` for the dev_id. I instead made a
`static int` with the value `0` and used that instead. This worked.

The result was the following:
```
[10530.315278] lab1_driver is initialised
[10532.146353] ISR[10] called 1 times!
[10534.150441] ISR[10] called 2 times!
[10536.154402] ISR[10] called 3 times!
[10538.159873] ISR[10] called 4 times!
[10540.162390] ISR[10] called 5 times!
[10542.166435] ISR[10] called 6 times!
[10544.170456] ISR[10] called 7 times!
[10546.174464] ISR[10] called 8 times!
[10548.178511] ISR[10] called 9 times!
[10550.182391] ISR[10] called 10 times!
[10552.186228] ISR[10] called 11 times!
[10554.190606] ISR[10] called 12 times!
[10556.194404] ISR[10] called 13 times!
[10558.198147] ISR[10] called 14 times!
[10560.202182] ISR[10] called 15 times!
[10562.206388] ISR[10] called 16 times!
[10564.210384] ISR[10] called 17 times!
[10566.214246] ISR[10] called 18 times!
[10568.218398] ISR[10] called 19 times!
[10570.222413] ISR[10] called 20 times!
[10572.226467] ISR[10] called 21 times!
[10574.236176] ISR[10] called 22 times!
[10575.036398] lab1_driver is exiting
[10575.036478] lab1_driver has exited
```

I still have no idea what is polling every 2 seconds (and triggers the interrupt
in the process) but this at least seemed to work nicely!

### Lab 2
Next up was writing a driver which tries to listen to all possible interrupts
and keeps track of how many calls each interrupt has had. Hence I wrote a driver
for this, without the read functionality. (`chapter8/lab2`) Instead I let it print the results upon
`driver_exit`, so that I could contain functionality withing the driver This
seemed to work pretty well:

```
[18127.125738] lab2_driver is initialising
[18127.125746] genirq: Flags mismatch irq 0. 00000080 (interrupt_tracker) vs. 00015a00 (timer)
[18127.125753] Listening for IRQ[1]
[18127.125755] genirq: Flags mismatch irq 2. 00000080 (interrupt_tracker) vs. 00010000 (cascade)
[18127.125763] Listening for IRQ[3]
[18127.125767] Listening for IRQ[4]
[18127.125771] Listening for IRQ[5]
[18127.125776] Listening for IRQ[6]
[18127.125780] Listening for IRQ[7]
[18127.125782] genirq: Flags mismatch irq 8. 00000080 (interrupt_tracker) vs. 00000000 (rtc0)
[18127.125785] Listening for IRQ[9]
[18127.125788] Listening for IRQ[10]
[18127.125791] Listening for IRQ[11]
[18127.125793] Listening for IRQ[12]
[18127.125798] Listening for IRQ[13]
[18127.125800] Listening for IRQ[14]
[18127.125802] Listening for IRQ[15]
[18127.128479] lab2_driver is initialised
[18226.969731] lab2_driver is exiting
[18226.971891] IRQ[1] had 28 calls
[18226.971898] IRQ[3] had 0 calls
[18226.971920] IRQ[4] had 0 calls
[18226.971924] IRQ[5] had 0 calls
[18226.971928] IRQ[6] had 0 calls
[18226.971932] IRQ[7] had 0 calls
[18226.971935] IRQ[9] had 96 calls
[18226.971938] IRQ[10] had 50 calls
[18226.971940] IRQ[11] had 20 calls
[18226.971943] IRQ[12] had 16 calls
[18226.971944] IRQ[13] had 0 calls
[18226.971948] IRQ[14] had 0 calls
[18226.971950] IRQ[15] had 96 calls
[18226.971952] lab2_driver has exited
```

I would assume that the flag mismatches are because the interrupts don't want
to be shared.

## Chapter 9
### Lab 1
This time I had to write a kernel module which uses a function of another kernel
module. So I wrote 2 modules:
* lab1_driver
* lab1_honk_driver

The honk driver has a function `HONK` which prints "HONK HONK" in the kernel log.
The other driver has the declaration `extern void HONK(void)` so that it can
attempt to use this honk function in its init.

Upon trying to load the main driver, the following came up: `[20713.980271] lab1_driver: Unknown symbol HONK (err 0)`
This was because the honk_driver wasn't loaded. after loading the honk module,
the following happened upon loading the main driver.

```
[20730.654395] honk_driver is initialising
[20730.654400] honk_driver is initialised
[20734.181859] lab1_driver is initialising
[20734.181864] HONK HONK from the honk module
[20734.181865] lab1_driver is initialised
```

Sources can be found in `chapter9/lab1`.

### Lab 2
Next up was duplicating the honk driver.  
First of all, during compilation the following warning arose:  
```
make[1]: Entering directory `/usr/src/linux-headers-3.19.0-32-generic'
  CC [M]  /media/mafn/Driver/chapter9/lab2/lab1_driver.o
  CC [M]  /media/mafn/Driver/chapter9/lab2/lab1_honk_driver.o
  CC [M]  /media/mafn/Driver/chapter9/lab2/lab1_honk_driver0.o
  Building modules, stage 2.
  MODPOST 3 modules
WARNING: /media/mafn/Driver/chapter9/lab2/lab1_honk_driver0: 'HONK' exported twice. Previous export was in /media/mafn/Driver/chapter9/lab2/lab1_honk_driver.ko
  CC      /media/mafn/Driver/chapter9/lab2/lab1_driver.mod.o
  LD [M]  /media/mafn/Driver/chapter9/lab2/lab1_driver.ko
  CC      /media/mafn/Driver/chapter9/lab2/lab1_honk_driver.mod.o
  LD [M]  /media/mafn/Driver/chapter9/lab2/lab1_honk_driver.ko
  CC      /media/mafn/Driver/chapter9/lab2/lab1_honk_driver0.mod.o
  LD [M]  /media/mafn/Driver/chapter9/lab2/lab1_honk_driver0.ko
make[1]: Leaving directory `/usr/src/linux-headers-3.19.0-32-generic'
```

Loading both honk modules at the same time yielded the following:  

dmesg:  
```
[22908.172992] lab1_honk_driver0: exports duplicate symbol HONK (owned by lab1_honk_driver)
```

terminal:  
```
mafn@0xFF /media/mafn/Driver/chapter9/lab2 $ sudo insmod lab1_honk_driver0.ko
insmod: ERROR: could not insert module lab1_honk_driver0.ko: Invalid module format
```

Next up was installing the modules with make_install. Since my Makefile doesn't
automatically invoke the kernel makefile, I had to add another rule:
```
modules_install:
	$(KMAKE) modules_install
```

```
mafn@0xFF /media/mafn/Driver/chapter9/lab2 $ sudo make modules_install
/usr/bin/make -C /lib/modules/3.19.0-32-generic/build M=/media/mafn/Driver/chapter9/lab2 modules_install
make[1]: Entering directory `/usr/src/linux-headers-3.19.0-32-generic'
  INSTALL /media/mafn/Driver/chapter9/lab2/lab1_driver.ko
Can't read private key
  INSTALL /media/mafn/Driver/chapter9/lab2/lab1_honk_driver.ko
Can't read private key
  INSTALL /media/mafn/Driver/chapter9/lab2/lab1_honk_driver0.ko
Can't read private key
  DEPMOD  3.19.0-32-generic
make[1]: Leaving directory `/usr/src/linux-headers-3.19.0-32-generic'
```

I looked up what the private key stuff meant, but this apparently was not an issue.

I tried to figure out where it placed my kernel modules, but I couldn't find out
manually. So I went to my `/lib/modules/uname -r/` folder and did the following:
```
mafn@0xFF /lib/modules/3.19.0-32-generic $ find . -type f -name "lab1_driver.ko"
./extra/lab1_driver.ko
```

We can conclude that it places user made modules in the extra folder.

Next up was generating a modules.dep with `depmod`. `depmod -a`.

Doing a grep on the newly generated modules.dep revealed that my lab1 module was now added.

## Chapter 10
### Lab 1
I decided to put a kprobe in my driver, pointing to a function within my driver.
The kprobe was placed successfully, but for some reason the pre_handler never
got called.

I therefore tried placing the kprobe on a name of what the book suggested
"do_fork" instead. This seemed to work nicely, since as soon as the kprobe
was placed, this is what I saw in my `dmesg`.

```
[  210.511808] Hello from kprobe
[  210.511827] Hello from kprobe
[  210.532217] Hello from kprobe
[  210.532236] Hello from kprobe
[  210.533047] Hello from kprobe
[  210.533054] Hello from kprobe
[  210.550969] Hello from kprobe
[  210.550983] Hello from kprobe
```

Apparently do_fork is called **a lot**.

### Lab 2
I decided to use a jprobe on my own function this time. I therefore took the old
honk module (which exports a `HONK` function!) and insmodded it. I learned on
the internet that you can do `cat /proc/kallsyms` to find out all linux kernel
symbols. Upon doing `cat /proc/kallsyms | grep "HONK"` I could confirm that HONK
was a symbol.

My jprobe now looked like this:
```
static void honk_interceptor(void)
{
    times_called++;

    jprobe_return();
}

/* JPROBE */
static struct jprobe honk_probe = {
    .entry = honk_interceptor,
    .kp = {
        .symbol_name = "HONK"
    }
};
```

I made a HONK call in the driver's open entry point, so I could invoke HONK,
and made my driver print the amount of HONK calls upon exit. The result looked
good.

```
[ 2088.469737] lab2_driver is initialising
[ 2088.493174] lab2_driver is initialised
[ 2102.575824] lab2_driver is opening!
[ 2102.575842] HONK HONK from the honk module
[ 2102.576189] lab2_driver is closing!
[ 2107.801007] lab2_driver is opening!
[ 2107.801016] HONK HONK from the honk module
[ 2107.801349] lab2_driver is closing!
[ 2120.627770] lab2_driver is exiting
[ 2120.627784] Amount of HONK calls 2
[ 2120.629866] lab2_driver has exited
```

As we can see the amount of HONK calls corresponds to how many times the HONK
function was called.

### Lab 3
Turns out that only here I would have to take my own driver and jprobe it. But
I already did this in Lab 2 instead...

### Lab 4
Debugfs was already mounted, so no need to go through that. `mount | grep "debugfs"`.  
However to get into the debugfs, mounted at /sys/kernel/debug, I had to be root
I couldn't sudo cd into it since cd is not an actual program. So I did `su` instead.

I made my driver setup a debugfs which looks like this:

```c
/* DEBUGFS */
static void init_debugfs(void)
{
    debugfs_dir = debugfs_create_dir(DRIVER_NAME, NULL);
    if (!debugfs_dir) {
        printk("Could not create debugfs_dir\n");
    }

    debugfs_string = debugfs_create_file("test_string", 0644, debugfs_dir, &some_string, NULL);
    if (!debugfs_string) {
        printk("Could not create debugfs_string\n");
    }

    debugfs_times_opened = debugfs_create_u32("times_opened", 0644, debugfs_dir, &times_opened);
    if (!debugfs_times_opened) {
        printk("Could not create debugfs_times_opened\n");
    }
}

static void free_debugfs(void)
{
    debugfs_remove(debugfs_dir);
    debugfs_remove(debugfs_string);
    debugfs_remove(debugfs_times_opened);
}
```

This seemed to work nicely, except for the test_string:
```
0xFF debug # cd lab3_driver/
0xFF lab3_driver # ls
test_string  times_opened
0xFF lab3_driver # cat test_string
0xFF lab3_driver # vi test_string
0xFF lab3_driver # cat times_opened
0
0xFF lab3_driver # cat times_opened
3
```

It also seemed that the directory was not removed. This was most likely because
I tried to remove the directory **before** removing its contents, which resulted in
the directory failing to remove itself. I tried to remove the directory myself
as root, but then discovered that I couldn't. Which makes sense since this is kernel space!  
Reboot...

Even after fiddling around, I couldn't manage to write a string to a debugfs file.
unsigned ints worked fine though, and the debugfs dir I made now removed itself properly.

## Chapter 13
### Lab1
I made an ioctl driver and accompanying userspace program. However, the driver
would report that the size was wrong, when I clearly told it to read 1 byte.
Turns out that the size parameter expects a **type** and not an actual size.
The parameter later gets sizeof'd

After testing, both of these seemed to work rather well. Here are the results.

What the userspace program does is:  
read the driver's data (should be '-')  
write '!' to the driver

```
mafn@0xFF /media/mafn/Driver/chapter13/lab1 $ make insert
sudo insmod lab1_driver.ko
mafn@0xFF /media/mafn/Driver/chapter13/lab1 $ sudo ./lab1_userspace
[0] Performed ioctl
new userspace data: -
[0] Performed ioctl
new userspace data: !
mafn@0xFF /media/mafn/Driver/chapter13/lab1 $ make remove
sudo rmmod lab1_driver
mafn@0xFF /media/mafn/Driver/chapter13/lab1 $
```

### Lab 2
Next up was making the size of the transmission not a fixed size. I made the
driver write size * '!' upon a read. And to make it simply printk the contents
of a write.

I seemed to get this right the first time around, with this corresponding output.  

dmesg:
```
[19372.046149] lab2_driver is initialising
[19372.048153] lab2_driver is initialised
[19383.020751] lab2_driver is opening!
[19383.020760] lab2_driver ioctl: read - sending 5 bytes
[19383.020959] lab2_driver ioctl: read - sending 10 bytes
[19383.021067] lab2_driver ioctl: write - recieved 5 bytes - abcd
[19383.021122] lab2_driver ioctl: write - recieved 10 bytes - abcdefghi
[19383.021179] lab2_driver is closing!
```  

program:
```
mafn@0xFF /media/mafn/Driver/chapter13/lab2 $ sudo ./lab2_userspace
[0] Performed ioctl
new userspace data: !!!!
[0] Performed ioctl
new userspace data: !!!!!!!!!
[0] Performed ioctl
[0] Performed ioctl
```

The reason we see 4 and 9 characters sent and recieved is because C automatically
null terminates string constants.

### Lab 3
Next up was writing the signal sending program. Both the driver and userspace
application were pretty straightforward.

To test this I first tried to let the userspace program kill itself.
The output was as follows:

```
mafn@0xFF /media/mafn/Driver/chapter13/lab3 $ sudo ./lab3_userspace
[0] Performed ioctl for process_id
[0] Performed ioctl for setting signal
```

We can see that it has succeeded because it doesn't print out its final message.

Next up I tried having it close a process going on in another terminal (htop),
which I found out had pid 29154. 15 is SIGTERM, the friendly way of telling the
program to shut down:

```
mafn@0xFF /media/mafn/Driver/chapter13/lab3 $ sudo ./lab3_userspace 29154 15
[0] Performed ioctl for process_id
[0] Performed ioctl for setting signal
[0] Performed ioctl for sending signal
```

In my other terminal, I saw htop closing down immediatly.
Seems like we were successful.
