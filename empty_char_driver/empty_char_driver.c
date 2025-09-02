#include <linux/cdev.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/module.h>

#define DEVICE_NAME "empty_char_device"
#define NUM_DEVICES 1

static dev_t dev_num = 0;
static struct cdev empty_cdev;
static struct file_operations empty_cdev_fops = {
    .owner = THIS_MODULE
};
static struct class *cls;

/**
 * Module initialization entry point
 */
static int empty_char_driver_init(void) {
    pr_info("[empty_char_driver_init] Init Empty Char Driver...\n");
    /* Request kernel to allocates a range of char device numbers,
       the major number will be assgined dynamically. */
    int ret = alloc_chrdev_region(&dev_num, 0, NUM_DEVICES, DEVICE_NAME);
    if (ret != 0) {
        pr_err("[empty_char_driver_init] Failed to init Empty Char Driver...\n");
        return ret;
    }
    pr_info("[empty_char_driver_init] Create device number: <%d:%d>\n", MAJOR(dev_num), MINOR(dev_num));

    /* Iniitialize cdev variable */
    cdev_init(&empty_cdev, &empty_cdev_fops);
    empty_cdev.owner = THIS_MODULE;

    /* Register the char device to VFS */
    cdev_add(&empty_cdev, dev_num, NUM_DEVICES);

    /* Starting from kernel v6.4+, the owner argument was removed. */
    cls = class_create(/*THIS_MODULE, */DEVICE_NAME);
    
    device_create(cls, NULL, MKDEV(MAJOR(dev_num), 0), NULL, "ecd");
    
    pr_info("[empty_char_driver_init] Empty Char Driver Loaded...\n");

    return 0;
}

/** module clean-up entry point */
static void empty_module_exit(void) {
    pr_info("[empty_module_exit] Exit Empty Char Driver...\n");
    device_destroy(cls, dev_num);
    class_destroy(cls);
    cdev_del(&empty_cdev);
    /* Release the device numbers */
    unregister_chrdev_region(dev_num, NUM_DEVICES);
    pr_info("[empty_module_exit] Empty Char Driver Unloaded...\n");
}

/* Register module entry point to kernel */
module_init(empty_char_driver_init);
/* Register module clean-up entry point to kernel */
module_exit(empty_module_exit);

MODULE_DESCRIPTION("Linux Device Driver Getting Started");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Muheng Lee");
MODULE_LICENSE("GPL");