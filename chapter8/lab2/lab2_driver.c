#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/interrupt.h>

#define DRIVER_NAME "lab2_driver"
// From what I've seen in my VM, this goes up to 15 at this point in time.
// Just to be sure we set the max to 32.
#define MAX_INTERRUPT_NUMBER 32

static dev_t major_numbers;
static struct cdev *character_device;
static struct class* driver_class;
static int dev_id = 0;

static int IRQ_calls[MAX_INTERRUPT_NUMBER];


/* IRQ_calls convenience functions */
static void set_irq_invalid(int irq)
{
    IRQ_calls[irq] = -1;
}

static int irq_invalid(int irq)
{
    return IRQ_calls[irq] == -1;
}

static void reset_irq_call(int irq)
{
    IRQ_calls[irq] = 0;
}

static void init_irq_calls(void)
{
    int irq;

    for (irq=0; irq<MAX_INTERRUPT_NUMBER; irq++) {
        set_irq_invalid(irq);
    }
}

static void increment_irq_call(int irq)
{
    IRQ_calls[irq]++;
}

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

/* READ AND WRITE */
static ssize_t driver_read(struct file* file, char __user * user_buf, size_t user_buf_length, loff_t* ppos)
{
    printk(KERN_INFO "%s READ is not implemented\n", DRIVER_NAME);
    return 0;
}

static ssize_t driver_write(struct file* file, const char __user * user_buf, size_t user_buf_length, loff_t* ppos)
{
    printk(KERN_INFO "%s WRITE is not implemented\n", DRIVER_NAME);
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

/* IRQ HANDLERS */
static irqreturn_t interrupt_tracker(int irq, void* dev_id)
{
    increment_irq_call(irq);

    return IRQ_NONE;
}

/* INIT AND EXIT */
static void init_interrupts(void)
{
    int irq;
    int register_return_value;

    // Put all IRQ call entries to -1 (Error!)
    init_irq_calls();

    for(irq = 0; irq < MAX_INTERRUPT_NUMBER; irq++) {
        register_return_value = request_irq(
            irq, interrupt_tracker, IRQF_SHARED, "interrupt_tracker", &dev_id
        );

        // On a successful register, we set the IRQ call entry to 0 (not an error)
        if (register_return_value == 0) {
            reset_irq_call(irq);

            printk("Listening for IRQ[%d]\n", irq);
        }
    }
}

static void free_interrupts(void)
{
    int irq;

    for (irq = 0; irq < MAX_INTERRUPT_NUMBER; irq++) {
        if (!irq_invalid(irq)) {
            printk("IRQ[%d] had %d calls\n", irq, IRQ_calls[irq]);

            free_irq(irq, &dev_id);
        }
    }
}

static int __init some_init(void)
{
    printk(KERN_INFO "%s is initialising\n", DRIVER_NAME);

    // Fetch major number (Dynamically allocated)
    alloc_chrdev_region(&major_numbers, 0, 255, DRIVER_NAME);

    // Register our interrupt handlers to listen to all interrupts
    init_interrupts();

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

static void __exit some_exit(void)
{
    printk(KERN_INFO "%s is exiting\n", DRIVER_NAME);

    // Remove the device node
    device_destroy(driver_class, major_numbers);
    class_destroy(driver_class);

    // Delete the driver
    cdev_del(character_device);

    // Free the interrupts
    free_interrupts();

    // Free the major number
    unregister_chrdev_region(major_numbers, 1);

    printk(KERN_INFO "%s has exited\n", DRIVER_NAME);
}

module_init(some_init);
module_exit(some_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("mafn");
MODULE_DESCRIPTION("Chapter 8 lab2 driver");
MODULE_VERSION("printk");
