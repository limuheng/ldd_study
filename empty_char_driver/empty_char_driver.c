#include <linux/cdev.h>
#include <linux/err.h>
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
static struct class* cls;
static struct device* dev;

/**
 * Module initialization entry point
 */
static int empty_char_driver_init(void) {
    pr_info("[empty_char_driver_init] Init Empty Char Driver...\n");
    /* Request kernel to allocates a range of char device numbers,
       the major number will be assgined dynamically. */
    int ret = alloc_chrdev_region(&dev_num, 0, NUM_DEVICES, DEVICE_NAME);
    if (ret != 0) {
        pr_err("[empty_char_driver_init] alloc_chrdev_region returns error: %d\n", -ret);
        goto quit;
    }
    pr_info("[empty_char_driver_init] Create device number: <%d:%d>\n", MAJOR(dev_num), MINOR(dev_num));

    /* Iniitialize cdev variable and register handlers */
    cdev_init(&empty_cdev, &empty_cdev_fops);
    empty_cdev.owner = THIS_MODULE;

    /* Register the char device to VFS */
    ret = cdev_add(&empty_cdev, dev_num, NUM_DEVICES);
    if (ret != 0) {
        pr_err("[empty_char_driver_init] cdev_add returns error: %d\n", -ret);
        goto unregister_cdev;
    }

    /* Starting from kernel v6.4+, the owner argument was removed. */
    cls = class_create(/*THIS_MODULE, */DEVICE_NAME);
    if (IS_ERR(cls)) {
        ret = PTR_ERR(cls);
        pr_err("[empty_char_driver_init] class_create returns error: %d\n", -ret);
        goto delete_cdev;
    }

    dev = device_create(cls, NULL, MKDEV(MAJOR(dev_num), 0), NULL, "ecd");
    if (IS_ERR(dev)) {
        ret = PTR_ERR(dev);
        pr_err("[empty_char_driver_init] device_create returns error: %d\n", -ret);
        goto destroy_class;
    }

    pr_info("[empty_char_driver_init] Empty Char Driver Loaded...\n");

    return 0;

destroy_class:
    class_destroy(cls);
delete_cdev:
    cdev_del(&empty_cdev);
unregister_cdev:
    unregister_chrdev_region(dev_num, NUM_DEVICES);
quit:
    return ret;
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