#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Provides the erg character device");

static char erg[] = "erg";
#define len_erg ((sizeof(erg)/sizeof(erg[0])) - 1)

static dev_t devnode;
static struct class *devclass = NULL;
static struct cdev devcdev;
static struct device *devdevice = NULL;

static int erg_open(struct inode *, struct file *);
static int erg_release(struct inode *, struct file *);
static ssize_t erg_read(struct file *, char *, size_t, loff_t *);
static ssize_t erg_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations dev_ops = {
	.owner = THIS_MODULE,
	.read = erg_read,
	.write = erg_write,
	.open = erg_open,
	.release = erg_release
};

int init_module() {
	int status;
	if((status = alloc_chrdev_region(&devnode, 0, 1, erg)) < 0) {
		printk(KERN_ALERT "Can't erg: %d\n", status);
		return status;
	}
	if(IS_ERR(devclass = class_create(THIS_MODULE, erg))) {
		printk(KERN_ALERT "Can't erg: %ld\n", PTR_ERR(devclass));
		return PTR_ERR(devclass);
	}
	cdev_init(&devcdev, &dev_ops);
	if((status = cdev_add(&devcdev, devnode, 1)) < 0) {
		printk(KERN_ALERT "Can't erg: %d\n", status);
		return status;
	}
	if(IS_ERR(devdevice = device_create(devclass, NULL, devnode, NULL, erg))) {
		printk(KERN_ALERT "Can't erg: %ld\n", PTR_ERR(devdevice));
		return PTR_ERR(devdevice);
	}
	printk(KERN_INFO "Ready to erg");
	printk(KERN_INFO "Ergergergergergergergergergergergergergerg\n");
	return 0;
}

void cleanup_module() {
	device_destroy(devclass, devnode);
	cdev_del(&devcdev);
	class_destroy(devclass);
	unregister_chrdev_region(devnode, 1);
	printk(KERN_INFO "erg.\n");
}

static int erg_open(struct inode *ino, struct file *fil) {
	fil->f_pos = 0;
	try_module_get(THIS_MODULE);
	return 0;
}

static int erg_release(struct inode *ino, struct file *fil) {
	module_put(THIS_MODULE);
	return 0;
}


static ssize_t erg_read(struct file *fil, char *buf, size_t len, loff_t *off) {
	register int amt = len;
	register loff_t pos = fil->f_pos;
	while(amt-- > 0) {
		put_user(erg[pos++], buf++);
		if(pos > len_erg - 1) pos = 0;
	}
	fil->f_pos = pos;
	return len;
}

static ssize_t erg_write(struct file *fil, const char *buf, size_t len, loff_t *off) {
	/* cool. */
	return len;
}

