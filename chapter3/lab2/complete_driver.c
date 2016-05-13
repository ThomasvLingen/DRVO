#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init some_init(void) {
    printk(KERN_INFO "complete driver is initialising\n");
    return 0;
}

static void __exit some_exit(void) {
    printk(KERN_INFO "complete driver is exitting\n");
}

module_init(some_init);
module_exit(some_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mafn");
MODULE_DESCRIPTION("Chapter 2 lab 2 driver");
MODULE_VERSION("printk");
