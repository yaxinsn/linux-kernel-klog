#ifndef __KLOG___
#define __KLOG___

#include <linux/proc_fs.h>

struct klog_ctx
{
    spinlock_t  lock;
    char*  log_buf;
    int logbuf_size;
	char*  log_head;
	char*  log_tail;
	int around;
	struct proc_dir_entry* log_entry;
    char* file_name;
	struct proc_dir_entry* parent_entry;

};


int klog_printk(struct klog_ctx* ctx,const char *fmt, ...);

int destroy_klog(struct klog_ctx* ctx );
struct klog_ctx*  create_klog(char* file_name, struct proc_dir_entry* proc_dir,size_t size);


#endif
