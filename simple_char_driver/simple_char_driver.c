#include <linux/cdev.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/module.h>

#include "simple_char_driver_impl.h"

/**
 * Module initialization entry point
 */
static int simple_char_driver_init(void) {
    pr_info("[%s] Init Simple Char Driver...\n", __func__);
    /* Request kernel to allocates a range of char device numbers,
       the major number will be assgined dynamically. */
    int ret = alloc_chrdev_region(&dev_num, 0, NUM_DEVICES, DEVICE_NAME);
    if (ret != 0) {
        pr_err("[%s] alloc_chrdev_region returns error: %d\n", __func__, -ret);
        goto quit;
    }
    pr_info("[%s] Create device number: <%d:%d>\n", __func__, MAJOR(dev_num), MINOR(dev_num));

    /* Iniitialize cdev variable */
    cdev_init(&simple_cdev, &simple_cdev_fops);
    simple_cdev.owner = THIS_MODULE;

    /* Register the char device to VFS */
    ret = cdev_add(&simple_cdev, dev_num, NUM_DEVICES);
    if (ret != 0) {
        pr_err("[%s] cdev_add returns error: %d\n", __func__, -ret);
        goto unregister_cdev;
    }

    /* Starting from kernel v6.4+, the owner argument was removed. */
    cls = class_create(/*THIS_MODULE, */DEVICE_NAME);
    if (IS_ERR(cls)) {
        ret = PTR_ERR(cls);
        pr_err("[%s] class_create returns error: %d\n", __func__, -ret);
        goto delete_cdev;
    }

    dev = device_create(cls, NULL, MKDEV(MAJOR(dev_num), 0), NULL, "scd");
    if (IS_ERR(dev)) {
        ret = PTR_ERR(dev);
        pr_err("[%s] device_create returns error: %d\n", __func__, -ret);
        goto destroy_class;
    }

    pr_info("[%s] Simple Char Driver Loaded...\n", __func__);

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
    pr_info("[%s] Exit Simple Char Driver...\n", __func__);
    device_destroy(cls, dev_num);
    class_destroy(cls);
    cdev_del(&simple_cdev);
    /* Release the device numbers */
    unregister_chrdev_region(dev_num, NUM_DEVICES);
    pr_info("[%s] Simple Char Driver Unloaded...\n", __func__);
}

/* Register module entry point to kernel */
module_init(simple_char_driver_init);
/* Register module clean-up entry point to kernel */
module_exit(simple_module_exit);

MODULE_DESCRIPTION("Linux Device Driver Getting Started");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Muheng Lee");
MODULE_LICENSE("GPL");