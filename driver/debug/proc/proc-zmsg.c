#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>


#define ZMSG_BUF_SIZE   1024


static char zmsg_buf[ZMSG_BUF_SIZE];
static int zmsg_r = 0;
static int zmsg_w = 0;
static int zmsg_r_for_read = 0;

struct proc_dir_entry *zmsg_entry;
static DECLARE_WAIT_QUEUE_HEAD(zmsg_waitq);


static int is_zmsg_buf_empty(void)
{
    return (zmsg_r == zmsg_w);
}

static int is_zmsg_buf_empty_for_read(void)
{
    return (zmsg_r_for_read == zmsg_w);
}

static int is_zmsg_buf_full(void)
{
    return ((zmsg_w+1) % ZMSG_BUF_SIZE == zmsg_r);
}

static void zmsg_putc(char c)
{
    if (is_zmsg_buf_full()) {
        zmsg_r = (zmsg_r+1) % ZMSG_BUF_SIZE;

        if ((zmsg_r_for_read+1) % ZMSG_BUF_SIZE == zmsg_r) {
            zmsg_r_for_read = zmsg_r;
        }
    }

    zmsg_buf[zmsg_w] = c;
    zmsg_w = (zmsg_w+1) % ZMSG_BUF_SIZE;

    wake_up_interruptible(&zmsg_waitq);
}

static int zmsg_getc(char *p)
{
    if (is_zmsg_buf_empty()) {
        return 0;
    }

    *p = zmsg_buf[zmsg_r];
    zmsg_r = (zmsg_r+1) % ZMSG_BUF_SIZE;

    return 1;
}

static int zmsg_getc_for_read(char *p)
{
    if (is_zmsg_buf_empty_for_read()) {
        return 0;
    }

    *p = zmsg_buf[zmsg_r_for_read];
    zmsg_r_for_read = (zmsg_r_for_read+1) % ZMSG_BUF_SIZE;

    return 1;
}

int zprintk(const char *fmt, ...)
{
    int i, n;
    char tmp_buf[ZMSG_BUF_SIZE];
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(tmp_buf, INT_MAX, fmt, ap);
    va_end(ap);

    for (i = 0; i < n; i++) {
        zmsg_putc(tmp_buf[i]);
    }

    return n;
}

EXPORT_SYMBOL(zprintk);


static int proc_zmsg_open(struct inode *inode, struct file *file)
{
    zmsg_r_for_read = zmsg_r;

    return 0;
}

static ssize_t proc_zmsg_read(struct file *file, char __user *buff, size_t size, loff_t *ppos)
{
    int error = 0;
    char c;
    int i = 0;

    if ((file->f_flags & O_NONBLOCK) && is_zmsg_buf_empty_for_read()) {
        return -EAGAIN;
    }

    error = wait_event_interruptible(zmsg_waitq, !is_zmsg_buf_empty_for_read());

    while (!error && zmsg_getc_for_read(&c) && i < size) {
        error = __put_user(c, buff);
        buff++;
        i++;
    }

    if (!error) {
        error = i;
    }

    return error;
}


static struct file_operations proc_zmsg_fops = {
    .owner = THIS_MODULE,
    .open = proc_zmsg_open,
    .read = proc_zmsg_read,
};


static __init int proc_zmsg_init(void)
{
	zmsg_entry = create_proc_entry("zmsg", S_IRUSR, &proc_root);
	if (zmsg_entry) {
		zmsg_entry->proc_fops = &proc_zmsg_fops;
    }

	return 0;
}

static __exit void proc_zmsg_exit(void)
{
	remove_proc_entry("zmsg", &proc_root);
}

module_init(proc_zmsg_init);
module_exit(proc_zmsg_exit);

MODULE_AUTHOR("Shell Ever <shellever@163.com>");
MODULE_DESCRIPTION("jz2440v2 proc zmsg driver");
MODULE_LICENSE("GPL");

