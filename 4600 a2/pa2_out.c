#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include "shared_buffer.h" // Include the shared buffer header file

MODULE_LICENSE("GPL");

#define DEVICE_NAME "pa2_out"

static int major_num;

static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);

static struct file_operations fops = {
    .read = device_read,
    .open = device_open,
    .release = device_release,
};

static int __init pa2_out_init(void)
{
    major_num = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_num < 0) {
        printk(KERN_ALERT "pa2_out: failed to initialize device\n");
        return major_num;
    }

    printk(KERN_INFO "pa2_out module has been loaded\n");
    return 0;
}

static void __exit pa2_out_exit(void)
{
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "pa2_out module has been unloaded\n");
}

static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "pa2_out: device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "pa2_out: device released\n");
    return 0;
}

static ssize_t device_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
    int bytes_read = 0;

    // sync with mutex
   printk(KERN_INFO "pa2_out: waiting on the lock\n");
    if (mutex_lock_interruptible(&shared_memory_mutex)) {
        printk(KERN_INFO "pa2_out: failed to acquire lock\n");
        printk(KERN_INFO "pa2_out: failed to read\n");
        return -EINTR;
    }
    printk(KERN_INFO "pa2_out: acquired the lock\n");

    printk(KERN_INFO "pa2_out: in the critical section\n");

    // Use the read_from_shared_buffer function to read data
    bytes_read = read_from_shared_buffer(buffer, length);

    printk(KERN_INFO "pa2_out: releasing the lock\n");
    mutex_unlock(&shared_memory_mutex);

    printk(KERN_INFO "pa2_out: device read %d bytes\n", bytes_read);
    return bytes_read;
}

module_init(pa2_out_init);
module_exit(pa2_out_exit);
