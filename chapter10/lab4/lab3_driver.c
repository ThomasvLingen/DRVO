#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/debugfs.h>

#define DRIVER_NAME "lab3_driver"

static dev_t major_minor_numbers;
static struct cdev *character_device;
static struct class* driver_class;

static struct dentry* debugfs_dir;
static struct dentry* debugfs_string;
static char* some_string = "foobar";
static struct dentry* debugfs_times_opened;

static unsigned int times_opened = 0;

/* DEBUGFS */
static void init_debugfs(void)
{
    debugfs_dir = debugfs_create_dir(DRIVER_NAME, NULL);
    if (!debugfs_dir) {
        printk("Could not create debugfs_dir\n");
    }

    debugfs_string = debugfs_create_file("test_string", 0644, debugfs_dir, some_string, NULL);
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
    debugfs_remove(debugfs_string);
    debugfs_remove(debugfs_times_opened);
    debugfs_remove(debugfs_dir);
}

/* OPEN AND CLOSE */
static int driver_open(struct inode* inode, struct file* file) {
    printk(KERN_INFO "%s is opening [%d]!\n", DRIVER_NAME, times_opened);
    times_opened++;
    return 0;
}

static int driver_close(struct inode* inode, struct file* file) {
    printk(KERN_INFO "%s is closing!\n", DRIVER_NAME);
    return 0;
}

/* FILE_OPERATIONS */
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close
};

/* INIT AND EXIT */
static int __init some_init(void) {
    printk(KERN_INFO "%s is initialising\n", DRIVER_NAME);

    // Fetch major number (Dynamically allocated)
    alloc_chrdev_region(&major_minor_numbers, 0, 255, DRIVER_NAME);

    // Launch the driver
    character_device = cdev_alloc();
    cdev_init(character_device, &fops);
    cdev_add(character_device, major_minor_numbers, 1);

    // Make a device node using udev
    driver_class = class_create(THIS_MODULE, "lab3_driver_class");
    device_create(driver_class, NULL, major_minor_numbers, "%s", DRIVER_NAME);

    // Init debugfs
    init_debugfs();

    printk(KERN_INFO "%s is initialised\n", DRIVER_NAME);
    return 0;
}

static void __exit some_exit(void) {
    printk(KERN_INFO "%s is exiting\n", DRIVER_NAME);

    // Remove the device node
    device_destroy(driver_class, major_minor_numbers);
    class_destroy(driver_class);

    // Delete the driver
    cdev_del(character_device);

    // Free the major number
    unregister_chrdev_region(major_minor_numbers, 1);

    // Free debugfs
    free_debugfs();

    printk(KERN_INFO "%s has exited\n", DRIVER_NAME);
}

module_init(some_init);
module_exit(some_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("mafn");
MODULE_DESCRIPTION("Chapter 4 lab1 driver");
MODULE_VERSION("printk");
