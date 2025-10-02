#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

static int major = 0;
static rwlock_t lock;
static char string[20] = "hello";

static ssize_t read_string(struct file *file, char __user *buf, size_t size,
                           loff_t *ppos) {
  int ret = 0;
  rwlock_read_lock(&lock);
  ret = copy_to_user(buf, string, strlen(string));
  rwlock_read_unlock(&lock);
  return ret;
}

static ssize_t write_string(struct file *file, const char __user *buf,
                            size_t size, loff_t *ppos) {
  int ret = 0;
  rwlock_write_lock(&lock);
  ret = copy_from_user(string, buf, size);
  rwlock_write_unlock(&lock);
  return ret;
}

static struct file_operations fops = {
    .owner = THIS_MODULE, .read = read_string, .write = write_string};

static int __init startup(void) {
  pr_info("String module!\n");
  rwlock_init(&lock);
  major = register_chrdev(major, "string", &fops);
  if (major < 0)
    return major;

  pr_info("Major number: %d\n", major);
  return 0;
}

static void __exit cleanup(void) { pr_info("Godbye world!\n"); }

module_init(startup);
module_exit(cleanup);