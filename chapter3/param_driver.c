#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int param_int = 255;

module_param(param_int, int, 0755);

static int __init some_init(void) {
    printk(KERN_INFO "Int parameter driver is initialising\n");
    printk(KERN_INFO "Value of the parameter during init = %d\n", param_int);
    return 0;
}

static void __exit some_exit(void) {
    printk(KERN_INFO "Value of the parameter during exit = %d\n", param_int);
    printk(KERN_INFO "Int parameter driver is exitting\n");
}

module_init(some_init);
module_exit(some_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mafn");
MODULE_DESCRIPTION("Chapter 2 driver");
MODULE_VERSION("printk");
