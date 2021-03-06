
//#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/vmalloc.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_bridge/ebtables.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/smp.h>
#include <linux/cpumask.h>
#include <net/sock.h>
#include "klog.h"

#include "hello_sock_opt.h"

char hello[10][20];


struct klog_ctx* sim_klog;

static int hello_set_ctl(struct sock *sk, int cmd, void __user *user, unsigned int len)
{
	int ret=0;
    int i;
	if (!ns_capable(sock_net(sk)->user_ns, CAP_NET_ADMIN))
		return -EPERM;
	//printk("%s: len is %d \n",__func__,len);
	switch (cmd) {
	case HELLO_LISA:
		if (copy_from_user(hello[0], user,20) != 0){
           
			return -EFAULT;
		}
		//klog_printk(sim_klog,"START:\n");
		
			 klog_printk_time(sim_klog,"%s\n",hello[0]);
		break;

	case HELLO_MONA:
		if (copy_from_user(hello[1], user,20) != 0){
            
			return -EFAULT;
		}
		klog_printk_time(sim_klog,"%s\n",hello[1]);
		break;

	default:
		printk("hello_set_ctl:  unknown request %i\n", cmd);
		ret = -EINVAL;
	}

	return ret;
}


static int hello_get_ctl(struct sock *sk, int cmd, void __user *user, int *len)
{
	int ret=0;

	if (!ns_capable(sock_net(sk)->user_ns, CAP_NET_ADMIN))
		return -EPERM;

	switch (cmd) {
	case HELLO_LISA:
		if (copy_to_user(user,hello[0] , 20) != 0)
		{
			break;
		}
		break;
	case HELLO_MONA:
		if (copy_to_user(user,hello[1] , 20) != 0)
			break;
		else
			break;

	default:
		printk("hello_get_ctl:  unknown request %i\n", cmd);
		ret = -EINVAL;
	}

	return ret;
}
static struct nf_sockopt_ops   arpt_sockopts = {
	.pf		= PF_INET,
	.set_optmin	= HELLO_BASE_CTL,
	.set_optmax	= HELLO_SO_SET_MAX+1,
	.set		= hello_set_ctl,
	.get_optmin	= HELLO_BASE_CTL,
	.get_optmax	= HELLO_SO_SET_MAX+1,
	.get		= hello_get_ctl,
	.owner		= THIS_MODULE,
};

static __init int simple_init(void)
{
	int ret;
	
	/* Register setsockopt */
	ret = nf_register_sockopt(&arpt_sockopts);
	if (ret < 0)
	{
		printk("nf_register_sockopt error! \n");
		return -1;
	}
	sim_klog = create_klog("simple_test",NULL,1);
    printk("hello !!\n");
    return 0;
}


static __exit void simple_cleanup(void)
{
	nf_unregister_sockopt(&arpt_sockopts);
    destroy_klog(sim_klog);

        printk("Hello byebye\n");
}


module_init(simple_init);
module_exit(simple_cleanup);

                                     
