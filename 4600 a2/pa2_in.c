#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include "shared_buffer.h"

MODULE_LICENSE("GPL");

#define DEVICE_NAME "pa2_in"

static int major_number;

static int device_open(struct inode *inode, struct file *file)
{
    pr_info("pa2_in: device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    pr_info("pa2_in: device released\n");
    return 0;
}

static ssize_t device_write(struct file *file, const char __user *buffer,
                             size_t length, loff_t *offset)
{
    ssize_t bytes_written;

    pr_info("pa2_in: write to device requested\n");

    pr_info("pa2_in: waiting on the lock\n");
    if (mutex_lock_interruptible(&shared_memory_mutex)) {
        pr_info("pa2_in: failed to acquire lock\n");
        return -EINTR;
    }
    pr_info("pa2_in: acquired the lock\n");

    pr_info("pa2_in: in the critical section\n");
    bytes_written = write_to_shared_buffer(buffer, length);
    if (bytes_written < 0) {
        pr_info("pa2_in: failed to write\n"); // Add this line
    } else {
        pr_info("pa2_in: message written to shared memory\n");
    }
    pr_info("pa2_in: releasing the lock\n");

    mutex_unlock(&shared_memory_mutex);
    return bytes_written;
}

static const struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .write = device_write,
};

static int __init pa2_in_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        pr_info("pa2_in: failed to initialize device\n");
        return major_number;
    }
    pr_info("pa2_in: module loaded\n");
    return 0;
}

static void __exit pa2_in_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("pa2_in: module unloaded\n");
}

module_init(pa2_in_init);
module_exit(pa2_in_exit);
