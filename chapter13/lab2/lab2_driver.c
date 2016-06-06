#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/slab.h>

#define DRIVER_NAME "lab2_driver"
// For IOCTL identification
#define DRIVER_TYPE '!'

static dev_t major_numbers;
static struct cdev *character_device;
static struct class* driver_class;

/* OPEN AND CLOSE */
static int driver_open(struct inode* inode, struct file* file)
{
    printk(KERN_INFO "%s is opening!\n", DRIVER_NAME);
    return 0;
}

static int driver_close(struct inode* inode, struct file* file)
{
    printk(KERN_INFO "%s is closing!\n", DRIVER_NAME);
    return 0;
}

/* IOCTL */
static long driver_ioctl (struct file* file, unsigned int command, unsigned long arguments)
{
    int return_value = 0;

    int type = _IOC_TYPE(command);
    int direction = _IOC_DIR(command);
    int size = _IOC_SIZE(command);
    void __user* user_space_pointer = (void __user*) arguments;

    // Allocate our kernel buffer
    char* kbuffer = kmalloc(size, GFP_KERNEL);

    if (type != DRIVER_TYPE) {
        printk(KERN_INFO "%s: invalid ioctl call. Wrong device type\n", DRIVER_NAME);
        return -EINVAL;
    }

    if (direction == _IOC_READ) {
        // Fill kbuffer with !'s to send
        memset(kbuffer, '!', size);

        printk(KERN_INFO "%s ioctl: read - sending %d bytes\n", DRIVER_NAME, size);

        return_value = copy_to_user(user_space_pointer, kbuffer, size);
    }
    else if (direction == _IOC_WRITE) {
        return_value = copy_from_user(kbuffer, user_space_pointer, size);

        printk(KERN_INFO "%s ioctl: write - recieved %d bytes - %s\n", DRIVER_NAME, size, kbuffer);
    } else {
        // If we get here, direction wasn't either READ or WRITE
        printk(KERN_INFO "%s ioctl: incorrect case\n", DRIVER_NAME);
        return EINVAL;
    }

    if (kbuffer) {
        kfree(kbuffer);
    }

    return return_value;
}

/* FILE_OPERATIONS */
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = driver_ioctl,
    .open = driver_open,
    .release = driver_close
};

/* INIT AND EXIT */
static int __init some_init(void) {
    printk(KERN_INFO "%s is initialising\n", DRIVER_NAME);

    // Fetch major number (Dynamically allocated)
    alloc_chrdev_region(&major_numbers, 0, 255, DRIVER_NAME);

    // Launch the driver
    character_device = cdev_alloc();
    cdev_init(character_device, &fops);
    cdev_add(character_device, major_numbers, 1);

    // Make a device node using udev
    driver_class = class_create(THIS_MODULE, "lab2_driver_class");
    device_create(driver_class, NULL, major_numbers, "%s", DRIVER_NAME);

    printk(KERN_INFO "%s is initialised\n", DRIVER_NAME);
    return 0;
}

static void __exit some_exit(void) {
    printk(KERN_INFO "%s is exiting\n", DRIVER_NAME);

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
MODULE_DESCRIPTION("Chapter 13 lab1 main driver");
MODULE_VERSION("printk");
