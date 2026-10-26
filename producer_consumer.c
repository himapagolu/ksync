#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");

static int prod = 2;
static int cons = 2;
static int size = 5;

module_param(prod, int, 0644);
module_param(cons, int, 0644);
module_param(size, int, 0644);

static struct semaphore empty;
static struct semaphore full;

static struct task_struct **producer_threads;
static struct task_struct **consumer_threads;

static int producer_function(void *data)
{
    int id = *(int *)data;
    
    printk(KERN_INFO "Producer-%d started\n", id);
    
    while (!kthread_should_stop()) {

        if (down_interruptible(&empty)) {
            
            if (kthread_should_stop())
                break;
            continue;
        }
        
        
        printk(KERN_INFO "An item has been produced by Producer-%d\n", id);
        up(&full);
        msleep(100);
    }
    
    printk(KERN_INFO "Producer-%d exiting\n", id);
    return 0;
}


static int consumer_function(void *data)
{
    int id = *(int *)data;
    
    printk(KERN_INFO "Consumer-%d started\n", id);
    
    while (!kthread_should_stop()) {
       
        if (down_interruptible(&full)) {
            if (kthread_should_stop())
                break;
            continue;
        }
        
        printk(KERN_INFO "An item has been consumed by Consumer-%d\n", id);
        up(&empty);
        msleep(100);
    }
    
    printk(KERN_INFO "Consumer-%d exiting\n", id);
    return 0;
}

static int __init producer_consumer_init(void)
{
    int i;
    int *id;
    
    printk(KERN_INFO "Initializing producer_consumer module\n");
    printk(KERN_INFO "Parameters: prod=%d, cons=%d, size=%d\n", prod, cons, size);
    
    
    if (prod < 0 || cons < 0 || size < 0) {
        printk(KERN_ERR "Invalid parameters: All parameters must be non-negative\n");
        return -EINVAL;
    }
    
    
    sema_init(&empty, size);
    sema_init(&full, 0);
    
    producer_threads = kmalloc(prod * sizeof(struct task_struct *), GFP_KERNEL);
    consumer_threads = kmalloc(cons * sizeof(struct task_struct *), GFP_KERNEL);
    
    if (!producer_threads || !consumer_threads) {
        printk(KERN_ERR "Failed to allocate memory\n");
        kfree(producer_threads);
        kfree(consumer_threads);
        return -ENOMEM;
    }
    
    for (i = 0; i < prod; i++) {
        id = kmalloc(sizeof(int), GFP_KERNEL);
        if (!id) {
            printk(KERN_ERR "Failed to allocate memory for producer ID\n");
            continue;
        }
        *id = i + 1;
        producer_threads[i] = kthread_run(producer_function, id, "Producer-%d", *id);
        if (IS_ERR(producer_threads[i])) {
            printk(KERN_ERR "Failed to create producer thread %d\n", i + 1);
            producer_threads[i] = NULL;
            kfree(id);
        }
    }
    
    for (i = 0; i < cons; i++) {
        id = kmalloc(sizeof(int), GFP_KERNEL);
        if (!id) {
            printk(KERN_ERR "Failed to allocate memory for consumer ID\n");
        
            continue;
        }
        *id = i + 1;
        consumer_threads[i] = kthread_run(consumer_function, id, "Consumer-%d", *id);
        if (IS_ERR(consumer_threads[i])) {
            printk(KERN_ERR "Failed to create consumer thread %d\n", i + 1);
            consumer_threads[i] = NULL;
            kfree(id);
        }
    }
    
    printk(KERN_INFO "producer_consumer module loaded\n");
    return 0;
}

static void __exit producer_consumer_exit(void)
{
    int i;
    
    printk(KERN_INFO "Unloading producer_consumer module\n");
    
    for (i = 0; i < prod; i++) {
        if (producer_threads[i]) {
            kthread_stop(producer_threads[i]);
            printk(KERN_INFO "Stopped Producer-%d\n", i + 1);
        }
    }
    
    for (i = 0; i < cons; i++) {
        if (consumer_threads[i]) {
            kthread_stop(consumer_threads[i]);
            printk(KERN_INFO "Stopped Consumer-%d\n", i + 1);
        }
    }
    
    kfree(producer_threads);
    kfree(consumer_threads);
    
    printk(KERN_INFO "producer_consumer module unloaded\n");
}

module_init(producer_consumer_init);
module_exit(producer_consumer_exit);

