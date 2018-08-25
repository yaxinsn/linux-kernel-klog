/*
说明：
这个模块，用于应用层传递一个TLV的mesg.
ohter kernel modules ---> this dev--up to--> 应用层。
这个模块就像一个串口一样，只保留一个msg，用户只有一直地持续读才能读出全部的msg.
没有用户读，则会丢掉之前的msg.

问题：两个进程同时读一个dev，本质上就是同时读一内存嘛，怎么办？

问题2：注意mmap。
问题3：是否需要移动file->f_pos。更串口一样，总是不移动，也不记录之前来的数据。要读就总是读打开文件之后的内容。
串口类型的msg 输出。


*/

#include <linux/module.h>

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
//#include <linux/smp_lock.h>
#include <asm/uaccess.h>
//#include <asm/nvram.h>




#define NVRAM_SIZE	8192
struct tlv_msg_dev{
	struct miscdevice misc_dev;
	void* msg_buf;
	
	//msg list
	int max_type;
};
static struct tlv_msg_dev g_tlv_msg_dev;



static ssize_t nvram_len;

static ssize_t read_nvram(struct file *file, char __user *buf,
			  size_t count, loff_t *ppos)
{
	unsigned int i;
	char __user *p = buf;

	if (!access_ok(VERIFY_WRITE, buf, count))
		return -EFAULT;
	if (*ppos >= nvram_len)
		return 0;
#if 0
	for (i = *ppos; count > 0 && i < nvram_len; ++i, ++p, --count)
		if (__put_user(nvram_read_byte(i), p))
			return -EFAULT;
#endif	
	*ppos = i;
	return p - buf;
}
#if 0
static ssize_t write_nvram(struct file *file, const char __user *buf,
			   size_t count, loff_t *ppos)
{
	unsigned int i;
	const char __user *p = buf;
	char c;

	if (!access_ok(VERIFY_READ, buf, count))
		return -EFAULT;
	if (*ppos >= nvram_len)
		return 0;
	for (i = *ppos; count > 0 && i < nvram_len; ++i, ++p, --count) {
		if (__get_user(c, p))
			return -EFAULT;
		nvram_write_byte(c, i);
	}
	*ppos = i;
	return p - buf;
}

static int nvram_ioctl(struct inode *inode, struct file *file,
	unsigned int cmd, unsigned long arg)
{
	switch(cmd) {

	case IOC_NVRAM_SYNC:
		nvram_sync();
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
#endif
static int tlv_open(struct inode *inode, struct file *file)
{
	file->private_data = &g_tlv_msg_dev;
	return 0;
}
const static struct file_operations cdev_fops = {
	.owner		= THIS_MODULE,
	.open       = tlv_open,
	//.llseek		= nvram_llseek,
	.read		= read_nvram,
	//.write		= write_nvram, 不让写。没有写操作。
	//.ioctl		= nvram_ioctl,
};

static struct tlv_msg_dev g_tlv_msg_dev = 
{
	{
		MISC_DYNAMIC_MINOR,
		"tlv_msg_cdev",
		&cdev_fops
	},
	NULL
};
int __init nvram_init(void)
{
	int ret = 0;

	printk(KERN_INFO "Generic non-volatile memory driver v\n");
	ret = misc_register(&g_tlv_msg_dev.misc_dev);
	if (ret != 0)
		goto out;
	
	

out:
	return ret;
}

void __exit nvram_cleanup(void)
{
        misc_deregister( &g_tlv_msg_dev.misc_dev );
}

module_init(nvram_init);
module_exit(nvram_cleanup);
MODULE_LICENSE("GPL");
