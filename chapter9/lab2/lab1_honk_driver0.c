#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DRIVER_NAME "honk_driver"

/* INIT AND EXIT */
static int __init some_init(void) {
    printk(KERN_INFO "%s is initialising\n", DRIVER_NAME);
    printk(KERN_INFO "%s is initialised\n", DRIVER_NAME);
    return 0;
}

static void __exit some_exit(void) {
    printk(KERN_INFO "%s is exiting\n", DRIVER_NAME);
    printk(KERN_INFO "%s has exited\n", DRIVER_NAME);
}

static void HONK(void) {
    printk(KERN_INFO "HONK HONK from the honk module");
}

EXPORT_SYMBOL(HONK);

module_init(some_init);
module_exit(some_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("mafn");
MODULE_DESCRIPTION("Chapter 9 lab1 honk driver");
MODULE_VERSION("printk");
