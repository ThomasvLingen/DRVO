#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/sched.h>

#define DRIVER_NAME "lab3_driver"
// For IOCTL identification
#define DRIVER_TYPE '!'

static dev_t major_numbers;
static struct cdev *character_device;
static struct class* driver_class;

/* IOCTL commands */
static unsigned int ioctl_set_process_target = _IO(DRIVER_TYPE, 1);
static unsigned int ioctl_set_signal = _IO(DRIVER_TYPE, 2);
static unsigned int ioctl_send_signal = _IO(DRIVER_TYPE, 3);

static struct task_struct* target = NULL;
static int signal_to_send = SIGTERM;

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
static void set_process_target(int process_id)
{
    target = pid_task(find_vpid(process_id), PIDTYPE_PID);
}

static void set_signal(int signal)
{
    signal_to_send = signal;
}

static int send_signal(void)
{
    int send_signal_return;

    if (!target) {
        printk(KERN_INFO "%s: Cannot send signal, no target has been set!\n", DRIVER_NAME);
        return -1;
    }

    send_signal_return = send_sig(signal_to_send, target, 0);
    printk(KERN_INFO "[%d] %s: %d sent", send_signal_return, DRIVER_NAME, signal_to_send);

    return 0;
}

static long driver_ioctl (struct file* file, unsigned int command, unsigned long arguments)
{
    int return_value = 0;

    int type = _IOC_TYPE(command);

    if (type != DRIVER_TYPE) {
        printk(KERN_INFO "%s: invalid ioctl call. Wrong device type\n", DRIVER_NAME);
        return -EINVAL;
    }

    if (command == ioctl_set_process_target) {
        set_process_target((int) arguments);
    }
    else if (command == ioctl_set_signal) {
        set_signal((int) arguments);
    }
    else if (command == ioctl_send_signal) {
        return_value = send_signal();
    } else {
        printk(KERN_INFO "%s: invalid ioctl call. Wrong command\n", DRIVER_NAME);
        return EINVAL;
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
    driver_class = class_create(THIS_MODULE, "lab3_driver_class");
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
MODULE_DESCRIPTION("Chapter 13 lab3 main driver");
MODULE_VERSION("printk");
