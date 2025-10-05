#ifndef __SIMPLE_CHAR_DRIVER_IMPL__
#define __SIMPLE_CHAR_DRIVER_IMPL__

#include <linux/rwlock.h>

#define DEVICE_NAME "simple_char_device"
#define NUM_DEVICES 1
#define BUFFER_SIZE 1024

extern dev_t dev_num;
extern struct cdev simple_cdev;
extern struct class *cls;
extern struct device* dev;
extern struct file_operations simple_cdev_fops;
extern char device_buffer[BUFFER_SIZE];

int simple_open(struct inode* inode, struct file* file);
int simple_close(struct inode* inode, struct file* file);
ssize_t simple_read(struct file* file, char __user* buffer, size_t length, loff_t* f_pos);
ssize_t simple_write(struct file* file, const char __user* buffer, size_t length, loff_t* f_pos);

#endif // __SIMPLE_CHAR_DRIVER_IMPL__