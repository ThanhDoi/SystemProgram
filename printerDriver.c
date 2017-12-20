#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/buffer_head.h>
#include <asm/segment.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("8B HEDSPI");
MODULE_DESCRIPTION("Printer Driver");
MODULE_VERSION("1.0");

static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class
static char pd_buffer[256]; // Memory for the string that is passed from userspace
static ssize_t buffer_size; // Used to remember the size of the string stored

/* The prototype functions for the character driver -- must come before the struct definition */
static int pd_open(struct inode *, struct file *);
static int pd_close(struct inode *, struct file *);
static ssize_t pd_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t pd_write(struct file *, const char __user *, size_t, loff_t *);
static int __init pd_init(void);
static void __exit pd_exit(void);

static struct file_operations pugs_fops = {
	.owner = THIS_MODULE,
	.open = pd_open,
	.release = pd_close,
	.read = pd_read,
	.write = pd_write
};

static int pd_open(struct inode *i, struct file *f) {
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}

static int pd_close(struct inode *i, struct file *f) {
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

static ssize_t pd_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
	printk(KERN_INFO "Read()\n");
	buffer_size = sizeof(pd_buffer);

	/* If position is behind the end of a file we have nothing to read */
	if (*off >= buffer_size) {
		return 0;
	}
	/* If a user tries to read more than we have, read only as many bytes as we have */
	if (*off + len > buffer_size) {
		len = buffer_size - *off;
	}

	/* Get data from pd_buffer */
	if (copy_to_user(buf, pd_buffer + *off, len) != 0)  {
		return -EFAULT;
	}

	/* Move reading position */
	*off += len;
	return len;
}

static ssize_t pd_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
	printk(KERN_INFO "Driver: write()\n");
	if (copy_from_user(pd_buffer, buf, len) != 0) {
		printk(KERN_ALERT "Error while writing\n");
		return -EFAULT;
	} else {
		pd_buffer[len] = '\0';
		printk(KERN_INFO "Write %zu bytes, message = %s\n", len, pd_buffer);
		return len;
	}
}

/* Constructor */
static int __init pd_init(void) {
	printk(KERN_INFO "Hello: printer driver registered\n");

	/*Try to dynamically allocate a major number for the device */
	if (alloc_chrdev_region(&first, 0, 1, "printerDevice") < 0) {
		printk(KERN_ALERT "Failed to register a major number!\n");
		return -1;
	}

	/* Register the device class */
	if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL) {
		unregister_chrdev_region(first, 1);
		printk(KERN_ALERT "Failed to register device class!\n");
		return -1;
	}

	/* Register the device driver */
	if (device_create(cl, NULL, first, NULL, "printerDevice") == NULL) {
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		printk(KERN_ALERT "Failed to create the device!\n");
		return -1;
	}

	/* Initialize a cdev structure */
	cdev_init(&c_dev, &pugs_fops);
	if (cdev_add(&c_dev, first, 1) == -1) {
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}

	return 0;
}

/* Destructor */
static void __exit pd_exit(void) {
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "Goodbye: printer driver unregistered\n");
}

module_init(pd_init);
module_exit(pd_exit);