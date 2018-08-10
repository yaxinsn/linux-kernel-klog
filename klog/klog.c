/*
 *
 * 
 *
 *
 *
 * */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/sockios.h>
#include <linux/jiffies.h>
#include <linux/times.h>

#include <linux/rculist.h>
#include <linux/rcupdate.h>
#include <linux/proc_fs.h>


#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/sockios.h>
#include <linux/jiffies.h>
#include <linux/times.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/netdevice.h>
#include <linux/string.h>


#include <net/ipv6.h>
#include <net/protocol.h>
#include <net/if_inet6.h>
#include <net/ndisc.h>
#include <net/ip.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>

#include <net/dsfield.h>  //for function  ipv4_change_dsfield
#include <linux/rculist.h>
#include <linux/rcupdate.h>
#include <linux/proc_fs.h>


struct klog_ctx{

	struct page* pages;
	void*  log_head;
	void* log_tail;

};

static int  klog_read (struct seq_file *m, void *v)
{
    struct klog_ctx*  t =(struct klog_ctx* )((struct seq_file*)m->private);

    return 0;
}

static int klog_open(struct inode *inode, struct file *file)
{
        return single_open(file,  klog_read, PDE(inode));
}

static int 
 klog_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{

    struct klog_ctx*  t =(struct klog_ctx* )((struct seq_file*)file->private_data)->private;
											

	char buf[20];
   	int a;
   	copy_from_user(&buf,buffer,20);
   	a = simple_strtol(buf,NULL,10);
   
   return count;


}	

static const struct file_operations klog_fops = {
		.open           = klog_open,
		.read           = seq_read,
		.llseek         = seq_lseek,
		.release        = single_release,
		.write          = klog_write,
};


int create_klog(char* file_name, struct proc_dir_entry* proc_dir,size_t size)
{

	struct klog_ctx* ctx = kmalloc(sizeof(struct klog_ctx),GFP_KERNEL);
	if(!ctx)
		return -1;

	
	char* log_buf = kmalloc(PAGE_SIZE*size,GFP_KERNEL);	
	if(!log_buf){
		kfree(ctx);
		printk("alloc pages is failed! \n");
		return -1;
	}

	proc_create_data(file_name, 0, proc_dir, &klog_fops, ctx);
	return 0;
}


EXPORT_SYMBOL(create_klog);

int destroy_klog(char* file_name,struct porc_dir_entry* proc_dir)
{
	return 0;
}

EXPORT_SYMBOL(destroy_klog);

static __init int klog_init()
{

	create_klog("test-klog",NULL,1)	;
	return 0;
}

__exit void klog_exit()
{
	printk("klog exit!\n");
	return;
}

module_init(klog_init);
module_exit(klog_exit);

MODULE_LICENSE("GPL");



