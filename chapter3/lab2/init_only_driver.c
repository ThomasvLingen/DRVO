#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init some_init(void) {
    printk(KERN_INFO "init only driver is initialising\n");
    return 0;
}

module_init(some_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mafn");
MODULE_DESCRIPTION("Chapter 2 lab 2 driver");
MODULE_VERSION("printk");
