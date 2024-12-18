#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/string.h>

#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/sysfs.h>

int i=0;
struct task_struct *ret;
static struct kobject* mymodule;
static int myvariable = 0;

int thread(void* data)
{
    while(1){
        printk(KERN_INFO "myvariable is equal %d ", myvariable);
        msleep(1000);
    if (myvariable==1){//run
        msleep(1000);
        i++;
        printk(KERN_INFO "It is %d second", i);
    }
    else if (myvariable==0)//pause
    {}
    else if (myvariable==-1)//reset
    {   
        i=0;
    }
    if (kthread_should_stop())
        break;
    }
    return 0;
}

static int myvariable_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf)
{
    return sprintf(buf, "%d\n", myvariable);
}

static ssize_t myvariable_store(struct kobject* kobj,
                                struct kobj_attribute* attr,
                                char* buf,
                                size_t count)
{
    sscanf(buf, "%du", &myvariable);
    return count;
}

static struct kobj_attribute myvariable_attribute=
    __ATTR(myvariable, 0660,(void*)myvariable_show,(void*)myvariable_store);

static int __init mymodule_init(void)
{
    int error=0;
    pr_info("mymodule initialised\n");
    mymodule=kobject_create_and_add("mymodule", kernel_kobj);
    if (!mymodule)
        return -ENOMEM;
    error=sysfs_create_file(mymodule, &myvariable_attribute.attr);
    if (error){
        pr_info("failed to create the myvariable file "
        "in /sys/kernel/mymodule\n");
    }
    return error;

    ret=kthread_run(thread, NULL,"foo kthread");
    return 0;
}

static void __exit mymodule_exit(void)
{
    pr_info("mymodule: Exit success\n");
    kobject_put(mymodule);
    kthread_stop(ret);
}

MODULE_LICENSE("GPL");
//int i=0;

int init_module(void)
{
    mymodule_init();
    printk(KERN_INFO "Driver is working");
    ret=kthread_run(thread, NULL,"foo kthread");
    return 0;
}

void cleanup_module(void){
    printk(KERN_INFO "Driver is died");
    kthread_stop(ret);
    mymodule_exit();
}
