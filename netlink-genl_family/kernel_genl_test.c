/*
 *
 * netlink的分支之一genl,我希望能做到上下同时进行通信。
 *
 *
 *
 * */


#include <linux/kernel.h>
#include <net/netlink.h>
#include <net/genetlink.h>

#include <linux/genetlink.h>
#include <uapi/linux/genetlink.h>


#include <net/sock.h>

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

#include "kernel_genl_test.h"





#define __dprintk(fmt, args...)  printk(KERN_ERR "%s:%d ::  " fmt, __func__,__LINE__, ## args)

/**********************************************************************/
typedef struct test_info{
	char name[64];
	u8   index;
	u32  status;
	
}test_info_st;

int g_resigter_pid = 0; //以后会有一个注册线程的一个函数。以后再说。

test_info_st g_test_info[3];



static int __test_get_devinfo_get_params(struct sk_buff *skb, struct genl_info *info)
{
	u16 id = -1;
	char* name;
		if (info->attrs[__TEST_ATTR_DEV_INDEX]) {
		id = nla_get_u16(info->attrs[__TEST_ATTR_DEV_INDEX]);
		__dprintk("id %d \n",id);		
		
	}

	if (info->attrs[__TEST_ATTR_DEV_NAME]) {
		name = nla_data(info->attrs[__TEST_ATTR_DEV_NAME]);
			__dprintk("name %s \n",name);	
	}
	
	return id;
}

/* 每一个命令的处理函数都分成三个部分：
	接收检查参数； 
	处理信息；（自己的功能部分）
	返回消息。
*/
	
static int test_get_devinfo(struct sk_buff *skb, struct genl_info *info)
{
	struct sk_buff *msg;
	int err = -EINVAL;
	u16 id;
/* fetch the attrs, from info */
	id = __test_get_devinfo_get_params(skb,info);
/* fetch the attrs, from info end */
	if(id == -1)
	{
		return -EINVAL;//参数不对。
	}
/*fill up the reply mrsg . :*/
#if 0
	msg = ctrl_build_family_msg(res, info->snd_pid, info->snd_seq,
				    CTRL_CMD_NEWFAMILY);
	if (IS_ERR(msg))
		return PTR_ERR(msg);
#endif
	return genlmsg_reply(msg, info);
}
static int test_set_devinfo(struct sk_buff *skb, struct genl_info *info)
{
	return 0;
}

static int test_dump_devinfo(struct sk_buff *skb,
	struct netlink_callback *cb)
{
	return 0;
}	
static struct genl_ops test_ops[] = {
	{
		.cmd = TEST_CMD_GET_DEVINFO,
		.doit = test_get_devinfo,
		.dumpit = test_dump_devinfo,
		.policy = test_policy,
		/* can be retrieved by unprivileged users */
	},
	{
		.cmd = TEST_CMD_SET_DEVINFO,
		.doit = test_set_devinfo,
		.policy = test_policy,
		.flags = GENL_ADMIN_PERM,
	},
};


static struct genl_family test_family = {
	.id		= 9999,
	.hdrsize	= 0,
	.name		= "MyTest",
	.version	= 1,
	.maxattr	= ____TEST_ATTR_MAX,
	.ops = test_ops,
	.n_ops = ARRAY_SIZE(test_ops),
};

static int __init test_nl_init(void)
{
	int rc;
	int i;

	for( i = 0; i < ARRAY_SIZE(g_test_info); i++)
	{
		g_test_info[i].index = i;
		g_test_info[i].status = i+100;
	}


	
	rc = genl_register_family(&test_family);
	if (rc)
		goto fail;

#if 0
	for (i = 0; i < ARRAY_SIZE(test_ops); i++) {
		rc = genl_register_ops(&test_family, &test_ops[i]);
		if (rc)
			goto fail;
	}
#endif
	return 0;

fail:
	genl_unregister_family(&test_family);
	return rc;
}

static void __exit test_nl_exit(void)
{
	genl_unregister_family(&test_family);
}

module_init(test_nl_init);
module_exit(test_nl_exit);

