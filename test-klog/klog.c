/*
 *
 * 本程序在/proc里生成一个日志文件，为一些内核模块提供另一种日志打印机制。
 * 类似于printk与dmesg ，但是为了不影响系统日志，所以在另一个文件里输出一些特别的日志。
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

#include "klog.h"



int __seq_vprintf(struct seq_file *m, int lenx, const char *f, va_list args)
{
	int len;
    int __min = m->size - m->count < lenx?m->size - m->count:lenx;
	if (m->count < m->size) {
		len = vsnprintf(m->buf + m->count,__min , f, args);
		if (m->count + len < m->size) {
			m->count += len;
			return 0;
		}
	}
	m->count = m->size;
	return -1;
}
int seq_nprintf(struct seq_file *m, int len, const char *f, ...)
{
	int ret;
	va_list args;

	va_start(args, f);
	ret = __seq_vprintf(m,len ,f, args);
	va_end(args);

	return ret;
}

static int  klog_read (struct seq_file *m, void *v)
{
    struct klog_ctx*  t =(struct klog_ctx* )((struct seq_file*)m->private);
    char* start;
    char* end;   

    
    if(t->around == 1)
    {
        start = t->log_head+1;
        if(start >= t->log_buf)
            start = t->log_buf;
        end = t->log_head;
        
        printk("klog_read the around is 1 \n");
        seq_nprintf(m,(t->logbuf_size - (start - t->log_buf)),"%s",start);
        seq_nprintf(m,t->log_head - t->log_buf ,"%s",t->log_buf);
        
    }
    else
    {
        start = t->log_buf;
        end = t->log_head;
        
        seq_nprintf(m,t->log_head - t->log_buf,"%s",start);
    }
    
   // printk("ctx->log_buf %p  head %p  start %p \n",t->log_buf, t->log_head,start);
   // printk(" %s \n", start);

    
    return 0;
}

static int klog_open(struct inode *inode, struct file *file)
{
    return single_open(file,  klog_read, PDE_DATA(inode));
}

static ssize_t klog_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{

    struct klog_ctx*  t =(struct klog_ctx* )((struct seq_file*)file->private_data)->private;
											

	//char buf[20];
   	//int a;
   	//copy_from_user(&buf,buffer,20);
   	//a = simple_strtol(buf,NULL,10);

   //t->log_tail = t->log_head;
   
   return count;


}	

static const struct file_operations klog_fops = {
		.open           = klog_open,
		.read           = seq_read,
		.llseek         = seq_lseek,
		.release        = single_release,
		.write          = klog_write,
};


struct klog_ctx*  create_klog(char* file_name, struct proc_dir_entry* proc_dir,size_t size)
{
    struct proc_dir_entry *proc_entry  = NULL;
	struct klog_ctx* ctx = kmalloc(sizeof(struct klog_ctx),GFP_KERNEL|__GFP_ZERO);
	if(!ctx)
		return NULL;

	
	char* log_buf = kmalloc(20*size,GFP_KERNEL|__GFP_ZERO);	
	if(!log_buf){
		printk("alloc pages is failed! \n");
		goto err1;
	}
    ctx->log_buf = log_buf;
    ctx->logbuf_size = 20*size;
    
	proc_entry = proc_create_data(file_name, 0, proc_dir, &klog_fops, ctx);
	if(proc_entry == NULL)
	{
	    
	    printk("proc_create_data error! \n");
	    goto err2;
	}
	ctx->file_name = kmalloc(strlen(file_name)+1,GFP_KERNEL|__GFP_ZERO);
	ctx->file_name[strlen(file_name)] = 0;
	if(NULL == ctx->file_name)
	{
	    printk("malloc filename error\n");
	    goto err3;
	    
	}
	strncpy(ctx->file_name,file_name,strlen(file_name));
    
	ctx->parent_entry = proc_dir;
	ctx->log_entry = proc_entry;
	spin_lock_init(&ctx->lock);
	ctx->around = 0;
	ctx->log_head = ctx->log_buf;
	    printk("ctx->file_name %s\n",ctx->file_name);
	return ctx;
err3:
    
    remove_proc_entry(file_name,proc_dir);
err2:
    kfree(log_buf);
err1:
	kfree(ctx);


	return NULL;
}

EXPORT_SYMBOL(create_klog);

#define __KFREE(x)  do{ \
    if(x)  kfree(x);   \
}while(0)

int destroy_klog(struct klog_ctx* ctx )
{
    printk("----ctx->file_name %s -----\n",ctx->file_name);
    remove_proc_entry(ctx->file_name,ctx->parent_entry);
    
    __KFREE(ctx->file_name);
    __KFREE(ctx->log_buf);
    __KFREE(ctx);
    
	return 0;
}

EXPORT_SYMBOL(destroy_klog);
/**************真正的打印函数*****/
void __log_store(struct klog_ctx* ctx,char* textbuf,int text_len)
{
    //check around;
    u64 remain = (ctx->logbuf_size - (ctx->log_head - ctx->log_buf));
    printk("size %d, (ctx->log_head - ctx->log_buf) %d \n",
    ctx->logbuf_size, (ctx->log_head - ctx->log_buf));
    if(text_len > remain)
    {
        memcpy(ctx->log_head,textbuf,remain);
        memcpy(ctx->log_buf,textbuf+remain,text_len - remain);
        ctx->log_head = ctx->log_buf+text_len - remain;
        ctx->around = 1;
        printk("the around is 1 \n");
        
    }
    else
    {
        memcpy(ctx->log_head,textbuf,text_len);
        ctx->log_head+= text_len;
    }
    //printk("ctx->log_buf %p  head %p \n",ctx->log_buf, ctx->log_head);
    return;
}
asmlinkage int _vprintk_emit(struct klog_ctx* ctx,
			    const char *fmt, va_list args)
{
    int this_cpu;
	int printed_len = 0;
	static char textbuf[1024];
	char *text = textbuf;
	size_t text_len;
	
	unsigned long flags;
	//local_irq_save(flags);
	this_cpu = smp_processor_id();
	spin_lock(&ctx->lock);
	text_len = vscnprintf(text, sizeof(textbuf), fmt, args);
	__log_store( ctx,textbuf,text_len);
	spin_unlock(&ctx->lock);
}
int klog_printk(struct klog_ctx* ctx,const char *fmt, ...)
{
    int r;
	va_list args;
	va_start(args, fmt);
	r = _vprintk_emit(ctx,fmt, args);
	va_end(args);
}

EXPORT_SYMBOL(klog_printk);

/*  init deinit: */
struct klog_ctx* g_ctx = NULL; 

static __init int klog_init(void)
{
#if 0
	struct klog_ctx* ctx = create_klog("test-klog",NULL,1);
	if(ctx != NULL)
	{
	    g_ctx = ctx;
	    klog_printk(ctx,"-----------------\n");
	    klog_printk(ctx,"123456789\n");
	}

	printk("--------\n");
#endif
	return 0;
}

__exit void klog_exit(void)
{
   // destroy_klog(g_ctx);
	printk("klog exit!\n");
	return;
}

module_init(klog_init);
module_exit(klog_exit);

//MODULE_LICENSE("GPL");



