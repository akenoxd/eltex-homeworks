#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

static rwlock_t lock;
static char string[20] = "hello";

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr,
                          char *buff) {
  ssize_t count;

  read_lock(&lock);
  count = scnprintf(buff, PAGE_SIZE, "%s\n", string);
  read_unlock(&lock);

  return count;
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buff, size_t size) {
  int rc = 0;

  if (size > 20)
    return -EINVAL;

  write_lock(&lock);
  rc = scnprintf(string, sizeof(string), "%s", buff);

  write_unlock(&lock);
  return size;
}

static struct kobject *string_kobj;
static struct kobj_attribute string_attribute =
    __ATTR(string, 0664, sysfs_show, sysfs_store);

static int __init startup(void) {
  int ret;

  pr_info("String module!\n");
  rwlock_init(&lock);

  string_kobj = kobject_create_and_add("string_module", kernel_kobj);
  if (!string_kobj) {
    pr_err("Failed to create kobject\n");
    return -ENOMEM;
  }

  ret = sysfs_create_file(string_kobj, &string_attribute.attr);
  if (ret) {
    pr_err("Failed to create sysfs file\n");
    kobject_put(string_kobj);
    return ret;
  }

  pr_info("Sysfs file created: /sys/kernel/string_module/string\n");
  return 0;
}

static void __exit cleanup(void) {
  kobject_put(string_kobj);
  pr_info("Goodbye world!\n");
}

module_init(startup);
module_exit(cleanup);