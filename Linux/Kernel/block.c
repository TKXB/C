//
// Created by root on 5/4/17.
// 阻止其他模块加载
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/notifier.h>

int
module_notifier(struct notifier_block *nb,
                unsigned long action, void *data);

struct notifier_block nb = {
        .notifier_call = module_notifier,
        .priority = INT_MAX
};

int
fake_init(void);
void
fake_exit(void);


int
module_notifier(struct notifier_block *nb,
                unsigned long action, void *data)
{
    struct module *module;
    unsigned long flags;
    // 定义锁。
    DEFINE_SPINLOCK(module_notifier_spinlock);

    module = data;
    printk(KERN_INFO "Processing the module: %s\n", module->name);

    //保存中断状态加锁。
    spin_lock_irqsave(&module_notifier_spinlock, flags);
    switch (module->state) {
        case MODULE_STATE_COMING:
            printk(KERN_INFO "Replacing init and exit functions: %s.\n",
                     module->name);
            // 偷天换日：篡改模块的初始函数与退出函数。
            module->init = fake_init;
            module->exit = fake_exit;
            break;
        default:
            break;
    }

    // 恢复中断状态解锁。
    spin_unlock_irqrestore(&module_notifier_spinlock, flags);

    return NOTIFY_DONE;
}


int
fake_init(void)
{
    printk(KERN_INFO "%s\n", "Fake init.");

    return 0;
}


void
fake_exit(void)
{
    printk(KERN_INFO "%s\n", "Fake exit.");

    return;
}

MODULE_LICENSE("Dual BSD/GPL");

static int hook_init(void)
{
    printk(KERN_EMERG "Hello, HOOK\n");
    register_module_notifier(&nb);
    return 0;
}

static void hook_exit(void)
{
    unregister_module_notifier(&nb);
    printk(KERN_INFO "Goodbye world!\n");
}
module_init(hook_init);
module_exit(hook_exit);
