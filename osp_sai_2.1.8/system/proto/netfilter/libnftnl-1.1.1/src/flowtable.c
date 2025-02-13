#include "internal.h"

#include <time.h>
#include <endian.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <inttypes.h>

#include <libmnl/libmnl.h>
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nf_tables.h>
#include <linux/netfilter.h>
#include <linux/netfilter_arp.h>

#include <libnftnl/flowtable.h>
#include <buffer.h>

struct nftnl_flowtable {
	struct list_head	head;
	const char		*name;
	const char		*table;
	int			family;
	uint32_t		hooknum;
	int32_t			prio;
	uint32_t		size;
	const char		**dev_array;
	uint32_t		dev_array_len;
	uint32_t		ft_flags;
	uint32_t		use;
	uint32_t		flags;
};

struct nftnl_flowtable *nftnl_flowtable_alloc(void)
{
	return calloc(1, sizeof(struct nftnl_flowtable));
}
EXPORT_SYMBOL(nftnl_flowtable_alloc);

void nftnl_flowtable_free(const struct nftnl_flowtable *c)
{
	int i;

	if (c->flags & (1 << NFTNL_FLOWTABLE_NAME))
		xfree(c->name);
	if (c->flags & (1 << NFTNL_FLOWTABLE_TABLE))
		xfree(c->table);
	if (c->flags & (1 << NFTNL_FLOWTABLE_DEVICES)) {
		for (i = 0; i < c->dev_array_len; i++)
			xfree(c->dev_array[i]);

		xfree(c->dev_array);
	}
	xfree(c);
}
EXPORT_SYMBOL(nftnl_flowtable_free);

bool nftnl_flowtable_is_set(const struct nftnl_flowtable *c, uint16_t attr)
{
	return c->flags & (1 << attr);
}
EXPORT_SYMBOL(nftnl_flowtable_is_set);

void nftnl_flowtable_unset(struct nftnl_flowtable *c, uint16_t attr)
{
	int i;

	if (!(c->flags & (1 << attr)))
		return;

	switch (attr) {
	case NFTNL_FLOWTABLE_NAME:
		xfree(c->name);
		break;
	case NFTNL_FLOWTABLE_TABLE:
		xfree(c->table);
		break;
	case NFTNL_FLOWTABLE_HOOKNUM:
	case NFTNL_FLOWTABLE_PRIO:
	case NFTNL_FLOWTABLE_USE:
	case NFTNL_FLOWTABLE_FAMILY:
	case NFTNL_FLOWTABLE_FLAGS:
		break;
	case NFTNL_FLOWTABLE_DEVICES:
		for (i = 0; i < c->dev_array_len; i++) {
			xfree(c->dev_array[i]);
			xfree(c->dev_array);
		}
		break;
	default:
		return;
	}

	c->flags &= ~(1 << attr);
}
EXPORT_SYMBOL(nftnl_flowtable_unset);

static uint32_t nftnl_flowtable_validate[NFTNL_FLOWTABLE_MAX + 1] = {
	[NFTNL_FLOWTABLE_HOOKNUM]	= sizeof(uint32_t),
	[NFTNL_FLOWTABLE_PRIO]		= sizeof(int32_t),
	[NFTNL_FLOWTABLE_FAMILY]	= sizeof(uint32_t),
	[NFTNL_FLOWTABLE_FLAGS]		= sizeof(uint32_t),
};

int nftnl_flowtable_set_data(struct nftnl_flowtable *c, uint16_t attr,
			     const void *data, uint32_t data_len)
{
	const char **dev_array;
	int len = 0, i;

	nftnl_assert_attr_exists(attr, NFTNL_FLOWTABLE_MAX);
	nftnl_assert_validate(data, nftnl_flowtable_validate, attr, data_len);

	switch(attr) {
	case NFTNL_FLOWTABLE_NAME:
		if (c->flags & (1 << NFTNL_FLOWTABLE_NAME))
			xfree(c->name);

		c->name = strdup(data);
		if (!c->name)
			return -1;
		break;
	case NFTNL_FLOWTABLE_TABLE:
		if (c->flags & (1 << NFTNL_FLOWTABLE_TABLE))
			xfree(c->table);

		c->table = strdup(data);
		if (!c->table)
			return -1;
		break;
	case NFTNL_FLOWTABLE_HOOKNUM:
		memcpy(&c->hooknum, data, sizeof(c->hooknum));
		break;
	case NFTNL_FLOWTABLE_PRIO:
		memcpy(&c->prio, data, sizeof(c->prio));
		break;
	case NFTNL_FLOWTABLE_FAMILY:
		memcpy(&c->family, data, sizeof(c->family));
		break;
	case NFTNL_FLOWTABLE_DEVICES:
		dev_array = (const char **)data;
		while (dev_array[len] != NULL)
			len++;

		if (c->flags & (1 << NFTNL_FLOWTABLE_DEVICES)) {
			for (i = 0; i < c->dev_array_len; i++) {
				xfree(c->dev_array[i]);
				xfree(c->dev_array);
			}
		}

		c->dev_array = calloc(len + 1, sizeof(char *));
		if (!c->dev_array)
			return -1;

		for (i = 0; i < len; i++)
			c->dev_array[i] = strdup(dev_array[i]);

		c->dev_array_len = len;
		break;
	case NFTNL_FLOWTABLE_SIZE:
		memcpy(&c->size, data, sizeof(c->size));
	case NFTNL_FLOWTABLE_FLAGS:
		memcpy(&c->ft_flags, data, sizeof(c->ft_flags));
		break;
	}
	c->flags |= (1 << attr);
	return 0;
}
EXPORT_SYMBOL(nftnl_flowtable_set_data);

void nftnl_flowtable_set(struct nftnl_flowtable *c, uint16_t attr, const void *data)
{
	nftnl_flowtable_set_data(c, attr, data, nftnl_flowtable_validate[attr]);
}
EXPORT_SYMBOL(nftnl_flowtable_set);

void nftnl_flowtable_set_u32(struct nftnl_flowtable *c, uint16_t attr, uint32_t data)
{
	nftnl_flowtable_set_data(c, attr, &data, sizeof(uint32_t));
}
EXPORT_SYMBOL(nftnl_flowtable_set_u32);

void nftnl_flowtable_set_s32(struct nftnl_flowtable *c, uint16_t attr, int32_t data)
{
	nftnl_flowtable_set_data(c, attr, &data, sizeof(int32_t));
}
EXPORT_SYMBOL(nftnl_flowtable_set_s32);

int nftnl_flowtable_set_str(struct nftnl_flowtable *c, uint16_t attr, const char *str)
{
	return nftnl_flowtable_set_data(c, attr, str, strlen(str) + 1);
}
EXPORT_SYMBOL(nftnl_flowtable_set_str);

const void *nftnl_flowtable_get_data(const struct nftnl_flowtable *c,
				     uint16_t attr, uint32_t *data_len)
{
	if (!(c->flags & (1 << attr)))
		return NULL;

	switch(attr) {
	case NFTNL_FLOWTABLE_NAME:
		*data_len = strlen(c->name) + 1;
		return c->name;
	case NFTNL_FLOWTABLE_TABLE:
		*data_len = strlen(c->table) + 1;
		return c->table;
	case NFTNL_FLOWTABLE_HOOKNUM:
		*data_len = sizeof(uint32_t);
		return &c->hooknum;
	case NFTNL_FLOWTABLE_PRIO:
		*data_len = sizeof(int32_t);
		return &c->prio;
	case NFTNL_FLOWTABLE_FAMILY:
		*data_len = sizeof(int32_t);
		return &c->family;
	case NFTNL_FLOWTABLE_DEVICES:
		return &c->dev_array[0];
	case NFTNL_FLOWTABLE_SIZE:
		*data_len = sizeof(int32_t);
		return &c->size;
	case NFTNL_FLOWTABLE_FLAGS:
		*data_len = sizeof(int32_t);
		return &c->ft_flags;
	}
	return NULL;
}
EXPORT_SYMBOL(nftnl_flowtable_get_data);

const void *nftnl_flowtable_get(const struct nftnl_flowtable *c, uint16_t attr)
{
	uint32_t data_len;
	return nftnl_flowtable_get_data(c, attr, &data_len);
}
EXPORT_SYMBOL(nftnl_flowtable_get);

const char *nftnl_flowtable_get_str(const struct nftnl_flowtable *c, uint16_t attr)
{
	return nftnl_flowtable_get(c, attr);
}
EXPORT_SYMBOL(nftnl_flowtable_get_str);

uint32_t nftnl_flowtable_get_u32(const struct nftnl_flowtable *c, uint16_t attr)
{
	uint32_t data_len;
	const uint32_t *val = nftnl_flowtable_get_data(c, attr, &data_len);

	nftnl_assert(val, attr, data_len == sizeof(uint32_t));

	return val ? *val : 0;
}
EXPORT_SYMBOL(nftnl_flowtable_get_u32);

int32_t nftnl_flowtable_get_s32(const struct nftnl_flowtable *c, uint16_t attr)
{
	uint32_t data_len;
	const int32_t *val = nftnl_flowtable_get_data(c, attr, &data_len);

	nftnl_assert(val, attr, data_len == sizeof(int32_t));

	return val ? *val : 0;
}
EXPORT_SYMBOL(nftnl_flowtable_get_s32);

void nftnl_flowtable_nlmsg_build_payload(struct nlmsghdr *nlh,
					 const struct nftnl_flowtable *c)
{
	int i;

	if (c->flags & (1 << NFTNL_FLOWTABLE_TABLE))
		mnl_attr_put_strz(nlh, NFTA_FLOWTABLE_TABLE, c->table);
	if (c->flags & (1 << NFTNL_FLOWTABLE_NAME))
		mnl_attr_put_strz(nlh, NFTA_FLOWTABLE_NAME, c->name);
	if ((c->flags & (1 << NFTNL_FLOWTABLE_HOOKNUM)) &&
	    (c->flags & (1 << NFTNL_FLOWTABLE_PRIO))) {
		struct nlattr *nest;

		nest = mnl_attr_nest_start(nlh, NFTA_FLOWTABLE_HOOK);
		mnl_attr_put_u32(nlh, NFTA_FLOWTABLE_HOOK_NUM, htonl(c->hooknum));
		mnl_attr_put_u32(nlh, NFTA_FLOWTABLE_HOOK_PRIORITY, htonl(c->prio));
		if (c->flags & (1 << NFTNL_FLOWTABLE_DEVICES)) {
			struct nlattr *nest_dev;

			nest_dev = mnl_attr_nest_start(nlh,
						       NFTA_FLOWTABLE_HOOK_DEVS);
			for (i = 0; i < c->dev_array_len; i++)
				mnl_attr_put_strz(nlh, NFTA_DEVICE_NAME,
						  c->dev_array[i]);
			mnl_attr_nest_end(nlh, nest_dev);
		}
		mnl_attr_nest_end(nlh, nest);
	}
	if (c->flags & (1 << NFTNL_FLOWTABLE_FLAGS))
		mnl_attr_put_u32(nlh, NFTA_FLOWTABLE_FLAGS, htonl(c->ft_flags));
	if (c->flags & (1 << NFTNL_FLOWTABLE_USE))
		mnl_attr_put_u32(nlh, NFTA_FLOWTABLE_USE, htonl(c->use));
	if (c->flags & (1 << NFTNL_FLOWTABLE_SIZE))
		mnl_attr_put_u32(nlh, NFTA_FLOWTABLE_SIZE, htonl(c->size));
}
EXPORT_SYMBOL(nftnl_flowtable_nlmsg_build_payload);

static int nftnl_flowtable_parse_attr_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_FLOWTABLE_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_FLOWTABLE_NAME:
	case NFTA_FLOWTABLE_TABLE:
		if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0)
			abi_breakage();
		break;
	case NFTA_FLOWTABLE_HOOK:
		if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0)
			abi_breakage();
		break;
	case NFTA_FLOWTABLE_FLAGS:
	case NFTA_FLOWTABLE_USE:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static int nftnl_flowtable_parse_hook_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_FLOWTABLE_HOOK_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_FLOWTABLE_HOOK_NUM:
	case NFTA_FLOWTABLE_HOOK_PRIORITY:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	case NFTA_FLOWTABLE_HOOK_DEVS:
		if (mnl_attr_validate(attr, MNL_TYPE_NESTED) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static int nftnl_flowtable_parse_devs(struct nlattr *nest,
				      struct nftnl_flowtable *c)
{
	struct nlattr *attr;
	char *dev_array[8];
	int len = 0, i;

	mnl_attr_for_each_nested(attr, nest) {
		if (mnl_attr_get_type(attr) != NFTA_DEVICE_NAME)
			return -1;
		dev_array[len++] = strdup(mnl_attr_get_str(attr));
		if (len >= 8)
			break;
	}

	if (!len)
		return -1;

	c->dev_array = calloc(len + 1, sizeof(char *));
	if (!c->dev_array)
		return -1;

	c->dev_array_len = len;

	for (i = 0; i < len; i++)
		c->dev_array[i] = strdup(dev_array[i]);

	return 0;
}

static int nftnl_flowtable_parse_hook(struct nlattr *attr, struct nftnl_flowtable *c)
{
	struct nlattr *tb[NFTA_FLOWTABLE_HOOK_MAX + 1] = {};
	int ret;

	if (mnl_attr_parse_nested(attr, nftnl_flowtable_parse_hook_cb, tb) < 0)
		return -1;

	if (tb[NFTA_FLOWTABLE_HOOK_NUM]) {
		c->hooknum = ntohl(mnl_attr_get_u32(tb[NFTA_FLOWTABLE_HOOK_NUM]));
		c->flags |= (1 << NFTNL_FLOWTABLE_HOOKNUM);
	}
	if (tb[NFTA_FLOWTABLE_HOOK_PRIORITY]) {
		c->prio = ntohl(mnl_attr_get_u32(tb[NFTA_FLOWTABLE_HOOK_PRIORITY]));
		c->flags |= (1 << NFTNL_FLOWTABLE_PRIO);
	}
	if (tb[NFTA_FLOWTABLE_HOOK_DEVS]) {
		ret = nftnl_flowtable_parse_devs(tb[NFTA_FLOWTABLE_HOOK_DEVS], c);
		if (ret < 0)
			return -1;
		c->flags |= (1 << NFTNL_FLOWTABLE_DEVICES);
	}

	return 0;
}

int nftnl_flowtable_nlmsg_parse(const struct nlmsghdr *nlh, struct nftnl_flowtable *c)
{
	struct nlattr *tb[NFTA_FLOWTABLE_MAX + 1] = {};
	struct nfgenmsg *nfg = mnl_nlmsg_get_payload(nlh);
	int ret = 0;

	if (mnl_attr_parse(nlh, sizeof(*nfg), nftnl_flowtable_parse_attr_cb, tb) < 0)
		return -1;

	if (tb[NFTA_FLOWTABLE_NAME]) {
		if (c->flags & (1 << NFTNL_FLOWTABLE_NAME))
			xfree(c->name);
		c->name = strdup(mnl_attr_get_str(tb[NFTA_FLOWTABLE_NAME]));
		if (!c->name)
			return -1;
		c->flags |= (1 << NFTNL_FLOWTABLE_NAME);
	}
	if (tb[NFTA_FLOWTABLE_TABLE]) {
		if (c->flags & (1 << NFTNL_FLOWTABLE_TABLE))
			xfree(c->table);
		c->table = strdup(mnl_attr_get_str(tb[NFTA_FLOWTABLE_TABLE]));
		if (!c->table)
			return -1;
		c->flags |= (1 << NFTNL_FLOWTABLE_TABLE);
	}
	if (tb[NFTA_FLOWTABLE_HOOK]) {
		ret = nftnl_flowtable_parse_hook(tb[NFTA_FLOWTABLE_HOOK], c);
		if (ret < 0)
			return ret;
	}
	if (tb[NFTA_FLOWTABLE_FLAGS]) {
		c->ft_flags = ntohl(mnl_attr_get_u32(tb[NFTA_FLOWTABLE_FLAGS]));
		c->flags |= (1 << NFTNL_FLOWTABLE_FLAGS);
	}
	if (tb[NFTA_FLOWTABLE_USE]) {
		c->use = ntohl(mnl_attr_get_u32(tb[NFTA_FLOWTABLE_USE]));
		c->flags |= (1 << NFTNL_FLOWTABLE_USE);
	}
	if (tb[NFTA_FLOWTABLE_SIZE]) {
		c->size = ntohl(mnl_attr_get_u32(tb[NFTA_FLOWTABLE_SIZE]));
		c->flags |= (1 << NFTNL_FLOWTABLE_SIZE);
	}

	c->family = nfg->nfgen_family;
	c->flags |= (1 << NFTNL_FLOWTABLE_FAMILY);

	return ret;
}
EXPORT_SYMBOL(nftnl_flowtable_nlmsg_parse);

static const char *nftnl_hooknum2str(int family, int hooknum)
{
	switch (family) {
	case NFPROTO_IPV4:
	case NFPROTO_IPV6:
	case NFPROTO_INET:
	case NFPROTO_BRIDGE:
		switch (hooknum) {
		case NF_INET_PRE_ROUTING:
			return "prerouting";
		case NF_INET_LOCAL_IN:
			return "input";
		case NF_INET_FORWARD:
			return "forward";
		case NF_INET_LOCAL_OUT:
			return "output";
		case NF_INET_POST_ROUTING:
			return "postrouting";
		}
		break;
	case NFPROTO_ARP:
		switch (hooknum) {
		case NF_ARP_IN:
			return "input";
		case NF_ARP_OUT:
			return "output";
		case NF_ARP_FORWARD:
			return "forward";
		}
		break;
	case NFPROTO_NETDEV:
		switch (hooknum) {
		case NF_NETDEV_INGRESS:
			return "ingress";
		}
		break;
	}
	return "unknown";
}

static inline int nftnl_str2hooknum(int family, const char *hook)
{
	int hooknum;

	for (hooknum = 0; hooknum < NF_INET_NUMHOOKS; hooknum++) {
		if (strcmp(hook, nftnl_hooknum2str(family, hooknum)) == 0)
			return hooknum;
	}
	return -1;
}

#ifdef JSON_PARSING
static int nftnl_jansson_parse_flowtable(struct nftnl_flowtable *c,
					 json_t *tree,
					 struct nftnl_parse_err *err)
{
	const char *name, *table, *hooknum_str;
	int32_t family, prio, hooknum;
	uint32_t size;
	json_t *root;

	root = nftnl_jansson_get_node(tree, "flowtable", err);
	if (root == NULL)
		return -1;

	name = nftnl_jansson_parse_str(root, "name", err);
	if (name != NULL)
		nftnl_flowtable_set_str(c, NFTNL_FLOWTABLE_NAME, name);

	if (nftnl_jansson_parse_family(root, &family, err) == 0)
		nftnl_flowtable_set_u32(c, NFTNL_FLOWTABLE_FAMILY, family);

	table = nftnl_jansson_parse_str(root, "table", err);

	if (table != NULL)
		nftnl_flowtable_set_str(c, NFTNL_FLOWTABLE_TABLE, table);

	if (nftnl_jansson_node_exist(root, "hooknum")) {
		if (nftnl_jansson_parse_val(root, "prio", NFTNL_TYPE_S32,
					  &prio, err) == 0)
			nftnl_flowtable_set_s32(c, NFTNL_FLOWTABLE_PRIO, prio);

		hooknum_str = nftnl_jansson_parse_str(root, "hooknum", err);
		if (hooknum_str != NULL) {
			hooknum = nftnl_str2hooknum(c->family, hooknum_str);
			if (hooknum == -1)
				return -1;
			nftnl_flowtable_set_u32(c, NFTNL_FLOWTABLE_HOOKNUM,
					       hooknum);
		}
	}
	if (nftnl_jansson_parse_val(root, "size", NFTNL_TYPE_U32,
				    &size, err) == 0)
		nftnl_flowtable_set_u32(c, NFTNL_FLOWTABLE_SIZE, size);

	return 0;
}
#endif

static int nftnl_flowtable_json_parse(struct nftnl_flowtable *c,
				      const void *json,
				      struct nftnl_parse_err *err,
				      enum nftnl_parse_input input)
{
#ifdef JSON_PARSING
	json_t *tree;
	json_error_t error;
	int ret;

	tree = nftnl_jansson_create_root(json, &error, err, input);
	if (tree == NULL)
		return -1;

	ret = nftnl_jansson_parse_flowtable(c, tree, err);

	nftnl_jansson_free_root(tree);

	return ret;
#else
	errno = EOPNOTSUPP;
	return -1;
#endif
}

static int nftnl_flowtable_do_parse(struct nftnl_flowtable *c,
				    enum nftnl_parse_type type,
				    const void *data,
				    struct nftnl_parse_err *err,
				    enum nftnl_parse_input input)
{
	int ret;
	struct nftnl_parse_err perr = {};

	switch (type) {
	case NFTNL_PARSE_JSON:
		ret = nftnl_flowtable_json_parse(c, data, &perr, input);
		break;
	case NFTNL_PARSE_XML:
	default:
		ret = -1;
		errno = EOPNOTSUPP;
		break;
	}

	if (err != NULL)
		*err = perr;

	return ret;
}

int nftnl_flowtable_parse(struct nftnl_flowtable *c, enum nftnl_parse_type type,
			  const char *data, struct nftnl_parse_err *err)
{
	return nftnl_flowtable_do_parse(c, type, data, err, NFTNL_PARSE_BUFFER);
}
EXPORT_SYMBOL(nftnl_flowtable_parse);

int nftnl_flowtable_parse_file(struct nftnl_flowtable *c,
			       enum nftnl_parse_type type,
			       FILE *fp, struct nftnl_parse_err *err)
{
	return nftnl_flowtable_do_parse(c, type, fp, err, NFTNL_PARSE_FILE);
}
EXPORT_SYMBOL(nftnl_flowtable_parse_file);

static int nftnl_flowtable_export(char *buf, size_t size,
				  const struct nftnl_flowtable *c, int type)
{
	NFTNL_BUF_INIT(b, buf, size);

	nftnl_buf_open(&b, type, CHAIN);
	if (c->flags & (1 << NFTNL_FLOWTABLE_NAME))
		nftnl_buf_str(&b, type, c->name, NAME);
	if (c->flags & (1 << NFTNL_FLOWTABLE_TABLE))
		nftnl_buf_str(&b, type, c->table, TABLE);
	if (c->flags & (1 << NFTNL_FLOWTABLE_FAMILY))
		nftnl_buf_str(&b, type, nftnl_family2str(c->family), FAMILY);
	if (c->flags & (1 << NFTNL_FLOWTABLE_USE))
		nftnl_buf_u32(&b, type, c->use, USE);
	if (c->flags & (1 << NFTNL_FLOWTABLE_HOOKNUM)) {
		if (c->flags & (1 << NFTNL_FLOWTABLE_HOOKNUM))
			nftnl_buf_str(&b, type, nftnl_hooknum2str(c->family,
					 c->hooknum), HOOKNUM);
		if (c->flags & (1 << NFTNL_FLOWTABLE_PRIO))
			nftnl_buf_s32(&b, type, c->prio, PRIO);
	}
	if (c->flags & (1 << NFTNL_FLOWTABLE_SIZE))
		nftnl_buf_u32(&b, type, c->size, SIZE);
	if (c->flags & (1 << NFTNL_FLOWTABLE_FLAGS))
		nftnl_buf_u32(&b, type, c->ft_flags, FLAGS);

	nftnl_buf_close(&b, type, CHAIN);

	return nftnl_buf_done(&b);
}

static int nftnl_flowtable_snprintf_default(char *buf, size_t size,
					    const struct nftnl_flowtable *c)
{
	int ret, remain = size, offset = 0, i;

	ret = snprintf(buf, remain, "flow table %s %s use %u size %u flags %x",
		       c->table, c->name, c->use, c->size, c->ft_flags);
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	if (c->flags & (1 << NFTNL_FLOWTABLE_HOOKNUM)) {
		ret = snprintf(buf + offset, remain, " hook %s prio %d ",
			       nftnl_hooknum2str(c->family, c->hooknum),
			       c->prio);
		SNPRINTF_BUFFER_SIZE(ret, remain, offset);

		if (c->flags & (1 << NFTNL_FLOWTABLE_DEVICES)) {
			ret = snprintf(buf + offset, remain, " dev { ");
			SNPRINTF_BUFFER_SIZE(ret, remain, offset);

			for (i = 0; i < c->dev_array_len; i++) {
				ret = snprintf(buf + offset, remain, " %s ",
					       c->dev_array[i]);
				SNPRINTF_BUFFER_SIZE(ret, remain, offset);
			}
			ret = snprintf(buf + offset, remain, " } ");
			SNPRINTF_BUFFER_SIZE(ret, remain, offset);
		}
	}

	return offset;
}

static int nftnl_flowtable_cmd_snprintf(char *buf, size_t size,
					const struct nftnl_flowtable *c,
					uint32_t cmd, uint32_t type,
					uint32_t flags)
{
	int ret, remain = size, offset = 0;

	ret = nftnl_cmd_header_snprintf(buf + offset, remain, cmd, type, flags);
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		ret = nftnl_flowtable_snprintf_default(buf + offset, remain, c);
		break;
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
		ret = nftnl_flowtable_export(buf + offset, remain, c, type);
		break;
	default:
		return -1;
	}

	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	ret = nftnl_cmd_footer_snprintf(buf + offset, remain, cmd, type, flags);
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	return offset;
}

int nftnl_flowtable_snprintf(char *buf, size_t size, const struct nftnl_flowtable *c,
			 uint32_t type, uint32_t flags)
{
	if (size)
		buf[0] = '\0';

	return nftnl_flowtable_cmd_snprintf(buf, size, c, nftnl_flag2cmd(flags),
					    type, flags);
}
EXPORT_SYMBOL(nftnl_flowtable_snprintf);

static int nftnl_flowtable_do_snprintf(char *buf, size_t size, const void *c,
				   uint32_t cmd, uint32_t type, uint32_t flags)
{
	return nftnl_flowtable_snprintf(buf, size, c, type, flags);
}

int nftnl_flowtable_fprintf(FILE *fp, const struct nftnl_flowtable *c,
			    uint32_t type, uint32_t flags)
{
	return nftnl_fprintf(fp, c, NFTNL_CMD_UNSPEC, type, flags,
			   nftnl_flowtable_do_snprintf);
}
EXPORT_SYMBOL(nftnl_flowtable_fprintf);

struct nftnl_flowtable_list {
	struct list_head list;
};

struct nftnl_flowtable_list *nftnl_flowtable_list_alloc(void)
{
	struct nftnl_flowtable_list *list;

	list = calloc(1, sizeof(struct nftnl_flowtable_list));
	if (list == NULL)
		return NULL;

	INIT_LIST_HEAD(&list->list);

	return list;
}
EXPORT_SYMBOL(nftnl_flowtable_list_alloc);

void nftnl_flowtable_list_free(struct nftnl_flowtable_list *list)
{
	struct nftnl_flowtable *s, *tmp;

	list_for_each_entry_safe(s, tmp, &list->list, head) {
		list_del(&s->head);
		nftnl_flowtable_free(s);
	}
	xfree(list);
}
EXPORT_SYMBOL(nftnl_flowtable_list_free);

int nftnl_flowtable_list_is_empty(const struct nftnl_flowtable_list *list)
{
	return list_empty(&list->list);
}
EXPORT_SYMBOL(nftnl_flowtable_list_is_empty);

void nftnl_flowtable_list_add(struct nftnl_flowtable *s,
			      struct nftnl_flowtable_list *list)
{
	list_add(&s->head, &list->list);
}
EXPORT_SYMBOL(nftnl_flowtable_list_add);

void nftnl_flowtable_list_add_tail(struct nftnl_flowtable *s,
				   struct nftnl_flowtable_list *list)
{
	list_add_tail(&s->head, &list->list);
}
EXPORT_SYMBOL(nftnl_flowtable_list_add_tail);

void nftnl_flowtable_list_del(struct nftnl_flowtable *s)
{
	list_del(&s->head);
}
EXPORT_SYMBOL(nftnl_flowtable_list_del);

int nftnl_flowtable_list_foreach(struct nftnl_flowtable_list *flowtable_list,
				 int (*cb)(struct nftnl_flowtable *t, void *data), void *data)
{
	struct nftnl_flowtable *cur, *tmp;
	int ret;

	list_for_each_entry_safe(cur, tmp, &flowtable_list->list, head) {
		ret = cb(cur, data);
		if (ret < 0)
			return ret;
	}
	return 0;
}
EXPORT_SYMBOL(nftnl_flowtable_list_foreach);
