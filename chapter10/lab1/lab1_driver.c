#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kprobes.h>

#define DRIVER_NAME "lab1_driver"

static dev_t major_numbers;
static struct cdev *character_device;
static struct class* driver_class;

static void zoodle(void)
{
    printk("zoodle");
}

EXPORT_SYMBOL(zoodle);

/* OPEN AND CLOSE */
static int driver_open(struct inode* inode, struct file* file)
{
    printk(KERN_INFO "%s is opening!\n", DRIVER_NAME);
    zoodle();
    return 0;
}

static int driver_close(struct inode* inode, struct file* file)
{
    printk(KERN_INFO "%s is closing!\n", DRIVER_NAME);
    return 0;
}

static int my_kprobe_pre_handler(struct kprobe* probe, struct pt_regs* registers)
{
    printk(KERN_INFO "Hello from kprobe\n");
    return 0;
}

static void my_kprobe_post_handler(struct kprobe* probe, struct pt_regs* registers, unsigned long flags)
{
    printk(KERN_INFO "Hello from kprobe\n");
}

/* FILE_OPERATIONS */
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close
};

/* KPROBE */
static struct kprobe test_probe = {
    .symbol_name = "do_fork",
    .pre_handler = my_kprobe_pre_handler,
    .post_handler = my_kprobe_post_handler
};

/* INIT AND EXIT */
static int __init some_init(void) {
    printk(KERN_INFO "%s is initialising\n", DRIVER_NAME);

    // Register kprobe
    if (register_kprobe(&test_probe)) {
        printk("Failed to register kprobe, quitting\n");
        return -1;
    }

    // Fetch major number (Dynamically allocated)
    alloc_chrdev_region(&major_numbers, 0, 255, DRIVER_NAME);

    // Launch the driver
    character_device = cdev_alloc();
    cdev_init(character_device, &fops);
    cdev_add(character_device, major_numbers, 1);

    // Make a device node using udev
    driver_class = class_create(THIS_MODULE, "lab1_driver_class");
    device_create(driver_class, NULL, major_numbers, "%s", DRIVER_NAME);

    printk(KERN_INFO "%s is initialised\n", DRIVER_NAME);
    return 0;
}

static void __exit some_exit(void) {
    printk(KERN_INFO "%s is exiting\n", DRIVER_NAME);

    // Unregister kprobe
    unregister_kprobe(&test_probe);

    // Remove the device node
    device_destroy(driver_class, major_numbers);
    class_destroy(driver_class);

    // Delete the driver
    cdev_del(character_device);

    // Free the major number
    unregister_chrdev_region(major_numbers, 1);

    printk(KERN_INFO "%s has exited\n", DRIVER_NAME);
}

module_init(some_init);
module_exit(some_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("mafn");
MODULE_DESCRIPTION("Chapter 10 lab1 main driver");
MODULE_VERSION("printk");
