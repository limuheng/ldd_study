#include <linux/cdev.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include "simple_char_driver_impl.h"

dev_t dev_num = 0;
struct cdev simple_cdev;
struct class* cls = NULL;
struct device* dev = NULL;
char device_buffer[BUFFER_SIZE] = {0};

struct file_operations simple_cdev_fops = {
    .owner = THIS_MODULE,
    .open = simple_open,
    .release = simple_close,
    .read = simple_read,
    .write = simple_write
};

int simple_open(struct inode* inode, struct file* file)
{
    pr_info("[%s] Open device file, major: %d, minor: %d\n", __func__, imajor(inode), iminor(inode));
    return 0;
}

int simple_close(struct inode* inode, struct file* file)
{
    pr_info("[%s] Close device file, major: %d, minor: %d\n", __func__, imajor(inode), iminor(inode));
    return 0;
}

ssize_t simple_read(struct file* file, char __user* buffer, size_t length, loff_t* f_pos)
{
    pr_info("[%s] request read %zu bytes\n", __func__, length);
    /* adjust the read bytes to not exceed the device buffer */
    if (*f_pos + length > BUFFER_SIZE) {
        length = BUFFER_SIZE - *f_pos;
    }

    if (copy_to_user(buffer, device_buffer + *f_pos, length)) {
        return -EFAULT;
    }
    pr_info("[%s] %zu bytes were read\n", __func__, length);

    /* update current file position */
    *f_pos += length;

    /* return number of bytes which have been successfully read */
    return length;
}

ssize_t simple_write(struct file* file, const char __user* buffer, size_t length, loff_t* f_pos)
{
    pr_info("[%s] request write %zu bytes\n", __func__, length);
    /* adjust the write bytes to not exceed the device buffer */
    if (*f_pos + length > BUFFER_SIZE) {
        length = BUFFER_SIZE - *f_pos;
    }
    
    if (length == 0) {
        return -ENOMEM;
    }

    if (copy_from_user(device_buffer + *f_pos, buffer, length)) {
        return -EFAULT;
    }
    pr_info("[%s] %zu bytes were written\n", __func__, length);

    /* update current file position */
    *f_pos += length;

    /* return number of bytes which have been successfully read */
    return length;
}

// ssize_t simple_read(struct file *, char __user *, size_t, loff_t *);
// ssize_t simple_write(struct file *, const char __user *, size_t, loff_t *);