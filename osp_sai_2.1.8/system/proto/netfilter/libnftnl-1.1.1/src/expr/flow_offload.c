#include "internal.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h> /* for memcpy */
#include <arpa/inet.h>
#include <errno.h>
#include <libmnl/libmnl.h>
#include <linux/netfilter/nf_tables.h>
#include <libnftnl/rule.h>
#include <libnftnl/expr.h>

struct nftnl_expr_flow {
	char			*table_name;
};

static int nftnl_expr_flow_set(struct nftnl_expr *e, uint16_t type,
			       const void *data, uint32_t data_len)
{
	struct nftnl_expr_flow *flow = nftnl_expr_data(e);

	switch (type) {
	case NFTNL_EXPR_FLOW_TABLE_NAME:
		flow->table_name = strdup((const char *)data);
		if (!flow->table_name)
			return -1;
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *nftnl_expr_flow_get(const struct nftnl_expr *e,
				       uint16_t type, uint32_t *data_len)
{
	struct nftnl_expr_flow *flow = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_FLOW_TABLE_NAME:
		*data_len = strlen(flow->table_name) + 1;
		return flow->table_name;
	}
	return NULL;
}

static int nftnl_expr_flow_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	int type = mnl_attr_get_type(attr);

	if (mnl_attr_type_valid(attr, NFTA_FLOW_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_FLOW_TABLE_NAME:
		if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void nftnl_expr_flow_build(struct nlmsghdr *nlh,
				  const struct nftnl_expr *e)
{
	struct nftnl_expr_flow *flow = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_FLOW_TABLE_NAME))
		mnl_attr_put_strz(nlh, NFTA_FLOW_TABLE_NAME, flow->table_name);
}

static int nftnl_expr_flow_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_flow *flow = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_FLOW_MAX+1] = {};
	int ret = 0;

	if (mnl_attr_parse_nested(attr, nftnl_expr_flow_cb, tb) < 0)
		return -1;

	if (tb[NFTA_FLOW_TABLE_NAME]) {
		flow->table_name =
			strdup(mnl_attr_get_str(tb[NFTA_FLOW_TABLE_NAME]));
		if (!flow->table_name)
			return -1;
		e->flags |= (1 << NFTNL_EXPR_FLOW_TABLE_NAME);
	}

	return ret;
}

static int
nftnl_expr_flow_json_parse(struct nftnl_expr *e, json_t *root,
				struct nftnl_parse_err *err)
{
#ifdef JSON_PARSING
	const char *table_name;

	table_name = nftnl_jansson_parse_str(root, "flowtable", err);
	if (table_name != NULL)
		nftnl_expr_set_str(e, NFTNL_EXPR_FLOW_TABLE_NAME, table_name);

	return 0;
#else
	errno = EOPNOTSUPP;
	return -1;
#endif
}

static int nftnl_expr_flow_export(char *buf, size_t size,
				  const struct nftnl_expr *e, int type)
{
	struct nftnl_expr_flow *l = nftnl_expr_data(e);
	NFTNL_BUF_INIT(b, buf, size);

	if (e->flags & (1 << NFTNL_EXPR_FLOW_TABLE_NAME))
		nftnl_buf_str(&b, type, l->table_name, SET);

	return nftnl_buf_done(&b);
}

static int nftnl_expr_flow_snprintf_default(char *buf, size_t size,
					    const struct nftnl_expr *e)
{
	int remain = size, offset = 0, ret;
	struct nftnl_expr_flow *l = nftnl_expr_data(e);

	ret = snprintf(buf, remain, "flowtable %s ", l->table_name);
	SNPRINTF_BUFFER_SIZE(ret, remain, offset);

	return offset;
}

static int nftnl_expr_flow_snprintf(char *buf, size_t size, uint32_t type,
				    uint32_t flags, const struct nftnl_expr *e)
{
	switch(type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_flow_snprintf_default(buf, size, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
		return nftnl_expr_flow_export(buf, size, e, type);
	default:
		break;
	}
	return -1;
}

static void nftnl_expr_flow_free(const struct nftnl_expr *e)
{
	struct nftnl_expr_flow *flow = nftnl_expr_data(e);

	xfree(flow->table_name);
}

static bool nftnl_expr_flow_cmp(const struct nftnl_expr *e1,
				const struct nftnl_expr *e2)
{
	struct nftnl_expr_flow *l1 = nftnl_expr_data(e1);
	struct nftnl_expr_flow *l2 = nftnl_expr_data(e2);
	bool eq = true;

	if (e1->flags & (1 << NFTNL_EXPR_FLOW_TABLE_NAME))
		eq &= !strcmp(l1->table_name, l2->table_name);

	return eq;
}

struct expr_ops expr_ops_flow = {
	.name		= "flow_offload",
	.alloc_len	= sizeof(struct nftnl_expr_flow),
	.max_attr	= NFTA_FLOW_MAX,
	.free		= nftnl_expr_flow_free,
	.cmp		= nftnl_expr_flow_cmp,
	.set		= nftnl_expr_flow_set,
	.get		= nftnl_expr_flow_get,
	.parse		= nftnl_expr_flow_parse,
	.build		= nftnl_expr_flow_build,
	.snprintf	= nftnl_expr_flow_snprintf,
	.json_parse	= nftnl_expr_flow_json_parse,
};
