#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DRIVER_NAME "char_driver"
#define DRIVER_MAJOR 700
#define DRIVER_MINOR 0

static dev_t major_minor_numbers;
static struct cdev *character_device;

/* OPEN AND CLOSE */
static int driver_open(struct inode* inode, struct file* file) {
    printk(KERN_INFO "char driver is opening!\n");
    return 0;
}

static int driver_close(struct inode* inode, struct file* file) {
    printk(KERN_INFO "char driver is closing!\n");
    return 0;
}

/* READ AND WRITE */
static ssize_t driver_read(struct file* file, char __user * user_buf, size_t user_buf_length, loff_t* ppos) {
    printk(KERN_INFO "char driver READ is not implemented\n");
    return 0;
}

static ssize_t driver_write(struct file* file, const char __user * user_buf, size_t user_buf_length, loff_t* ppos) {
    printk(KERN_INFO "char driver WRITE is not implemented\n");
    return user_buf_length;
}

/* FILE_OPERATIONS */
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write
};

/* INIT AND EXIT */
static int __init some_init(void) {
    printk(KERN_INFO "char driver is initialising\n");

    // Fetch major number
    major_minor_numbers = MKDEV(DRIVER_MAJOR, DRIVER_MINOR);
    register_chrdev_region(major_minor_numbers, 1, DRIVER_NAME);

    // Launch the driver
    character_device = cdev_alloc();
    cdev_init(character_device, &fops);
    cdev_add(character_device, major_minor_numbers, 1);

    printk(KERN_INFO "char driver is initialised\n");
    return 0;
}

static void __exit some_exit(void) {
    printk(KERN_INFO "char driver is exiting\n");

    // Delete the driver
    cdev_del(character_device);
    // Free the major number
    unregister_chrdev_region(major_minor_numbers, 1);

    printk(KERN_INFO "char driver has exited\n");
}

module_init(some_init);
module_exit(some_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("mafn");
MODULE_DESCRIPTION("Chapter 4 char driver");
MODULE_VERSION("printk");
