#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <asm/uaccess.h>
#include <asm/param.h>

#define BUFFER_SIZE 128

#define PROC_NAME "seconds"
#define MESSAGE "Seconds Timer\n"

/**
 * Function prototypes
 */
ssize_t seconds_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = seconds_read,
};

// Defining variable to store jiffies
unsigned long int initial_jiffies;

/* This function is called when the module is loaded. */
int seconds_init(void)
{

        // Reading initial Jiffy value on module load
        initial_jiffies = jiffies;

        // creates the /proc/seconds entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

        return 0;
}

/* This function is called when the module is removed. */
void seconds_exit(void)
{

        // removes the /proc/seconds entry
        remove_proc_entry(PROC_NAME, NULL);

        printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/seconds is read.
 * Using the template from hello.c with modifications
 */
ssize_t seconds_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        // Calculation of Elapsed time
        int elapsed_milliseconds = 1000 / HZ * (jiffies - initial_jiffies);
        int elapsed_seconds = elapsed_milliseconds / 1000;

        if (completed)
        {
                completed = 0;
                return 0;
        }

        completed = 1;

        // Printing and storing elapsed times
        rv = sprintf(buffer, "Elapsed time since module loaded: %ds %dms\n", elapsed_seconds, elapsed_milliseconds);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}

/* Macros for registering module entry and exit points. */
module_init(seconds_init);
module_exit(seconds_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("SGG");
