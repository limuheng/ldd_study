#include <linux/cdev.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/module.h>

#define DEVICE_NAME "simple_char_device"
#define NUM_DEVICES 1
#define BUFFER_SIZE 1024

//static char buffer[BUFFER_SIZE];
static dev_t dev_num = 0;
static struct cdev simple_cdev;

/*
static int simple_open(struct inode *, struct file *);
static int simple_release(struct inode *, struct file *);
static ssize_t simple_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t simple_write(struct file *, const char __user *, size_t, loff_t *);
*/

static struct file_operations simple_cdev_fops = {
    .owner = THIS_MODULE,
    /*
    .read = simple_read,
    .write = simple_write,
    .open = simple_open,
    .release = simple_release
    */
};

static struct class *cls;
static struct device* dev;

/**
 * Module initialization entry point
 */
static int simple_char_driver_init(void) {
    pr_info("[simple_char_driver_init] Init Simple Char Driver...\n");
    /* Request kernel to allocates a range of char device numbers,
       the major number will be assgined dynamically. */
    int ret = alloc_chrdev_region(&dev_num, 0, NUM_DEVICES, DEVICE_NAME);
    if (ret != 0) {
        pr_err("[simple_char_driver_init] alloc_chrdev_region returns error: %d\n", -ret);
        goto quit;
    }
    pr_info("[simple_char_driver_init] Create device number: <%d:%d>\n", MAJOR(dev_num), MINOR(dev_num));

    /* Iniitialize cdev variable */
    cdev_init(&simple_cdev, &simple_cdev_fops);
    simple_cdev.owner = THIS_MODULE;

    /* Register the char device to VFS */
    ret = cdev_add(&simple_cdev, dev_num, NUM_DEVICES);
    if (ret != 0) {
        pr_err("[simple_char_driver_init] cdev_add returns error: %d\n", -ret);
        goto unregister_cdev;
    }

    /* Starting from kernel v6.4+, the owner argument was removed. */
    cls = class_create(/*THIS_MODULE, */DEVICE_NAME);
    if (IS_ERR(cls)) {
        ret = PTR_ERR(cls);
        pr_err("[simple_char_driver_init] class_create returns error: %d\n", -ret);
        goto delete_cdev;
    }

    dev = device_create(cls, NULL, MKDEV(MAJOR(dev_num), 0), NULL, "scd");
    if (IS_ERR(dev)) {
        ret = PTR_ERR(dev);
        pr_err("[simple_char_driver_init] device_create returns error: %d\n", -ret);
        goto destroy_class;
    }

    pr_info("[simple_char_driver_init] Simple Char Driver Loaded...\n");

    return 0;

destroy_class:
    class_destroy(cls);
delete_cdev:
    cdev_del(&simple_cdev);
unregister_cdev:
    unregister_chrdev_region(dev_num, NUM_DEVICES);
quit:
    return ret;
}

/** module clean-up entry point */
static void simple_module_exit(void) {
    pr_info("[simple_module_exit] Exit Simple Char Driver...\n");
    device_destroy(cls, dev_num);
    class_destroy(cls);
    cdev_del(&simple_cdev);
    /* Release the device numbers */
    unregister_chrdev_region(dev_num, NUM_DEVICES);
    pr_info("[simple_module_exit] Simple Char Driver Unloaded...\n");
}

/* Register module entry point to kernel */
module_init(simple_char_driver_init);
/* Register module clean-up entry point to kernel */
module_exit(simple_module_exit);

MODULE_DESCRIPTION("Linux Device Driver Getting Started");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Muheng Lee");
MODULE_LICENSE("GPL");