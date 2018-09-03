#ifndef __KERNEL_GENL_TEST__
#define __KERNEL_GENL_TEST__
/* policy中的ATTR ID号 */
enum {
	____TEST_ATTR_INVALID,
	__TEST_ATTR_DEV_NAME,
	__TEST_ATTR_DEV_INDEX,
	__TEST_ATTR_STATUS,
	__TEST_ATTR_SHORT_ADDR,
	__TEST_ATTR_HW_ADDR,
	__TEST_ATTR_PAN_ID,
	__TEST_ATTR_PAGE,
	____TEST_ATTR_MAX,
};

/* ATTR 的type与len  */
const struct nla_policy test_policy[____TEST_ATTR_MAX + 1] = {
	[__TEST_ATTR_DEV_NAME] = { .type = NLA_STRING, },
	[__TEST_ATTR_DEV_INDEX] = { .type = NLA_U32, },
	[__TEST_ATTR_STATUS] = { .type = NLA_U8, },
	//[__TEST_ATTR_ED_LIST] = { .len = 27 },
};

/* 命令：  */
enum {
	__TEST_COMMAND_INVALID,
	TEST_CMD_GET_DEVINFO,
	TEST_CMD_SET_DEVINFO,
	__TEST_CMD_MAX,
};



#define TEST_CMD_MAX (__TEST_CMD_MAX - 1)

#endif //__KERNEL_GENL_TEST__