#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static void __exit some_exit(void) {
    printk(KERN_INFO "complete driver is exitting\n");
}

module_exit(some_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mafn");
MODULE_DESCRIPTION("Chapter 2 lab 2 driver");
MODULE_VERSION("printk");
