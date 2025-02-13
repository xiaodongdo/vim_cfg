/*
 * (C) 2016 by Pablo Neira Ayuso <pablo@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>

#include <linux/netfilter/nf_tables.h>

#include "internal.h"
#include <libmnl/libmnl.h>
#include <libnftnl/expr.h>
#include <libnftnl/rule.h>

struct nftnl_expr_objref {
	struct {
		uint32_t	type;
		const char	*name;
	} imm;
	struct {
		uint32_t	sreg;
		const char	*name;
		uint32_t	id;
	} set;
};

static int nftnl_expr_objref_set(struct nftnl_expr *e, uint16_t type,
				 const void *data, uint32_t data_len)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_OBJREF_IMM_TYPE:
		objref->imm.type = *((uint32_t *)data);
		break;
	case NFTNL_EXPR_OBJREF_IMM_NAME:
		objref->imm.name = strdup(data);
		if (!objref->imm.name)
			return -1;
		break;
	case NFTNL_EXPR_OBJREF_SET_SREG:
		objref->set.sreg = *((uint32_t *)data);
		break;
	case NFTNL_EXPR_OBJREF_SET_NAME:
		objref->set.name = strdup(data);
		if (!objref->set.name)
			return -1;
		break;
	case NFTNL_EXPR_OBJREF_SET_ID:
		objref->set.id = *((uint32_t *)data);
		break;
	default:
		return -1;
	}
	return 0;
}

static const void *nftnl_expr_objref_get(const struct nftnl_expr *e,
					 uint16_t type, uint32_t *data_len)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);

	switch(type) {
	case NFTNL_EXPR_OBJREF_IMM_TYPE:
		*data_len = sizeof(objref->imm.type);
		return &objref->imm.type;
	case NFTNL_EXPR_OBJREF_IMM_NAME:
		*data_len = strlen(objref->imm.name) + 1;
		return objref->imm.name;
	case NFTNL_EXPR_OBJREF_SET_SREG:
		*data_len = sizeof(objref->set.sreg);
		return &objref->set.sreg;
	case NFTNL_EXPR_OBJREF_SET_NAME:
		*data_len = strlen(objref->set.name) + 1;
		return objref->set.name;
	case NFTNL_EXPR_OBJREF_SET_ID:
		*data_len = sizeof(objref->set.id);
		return &objref->set.id;
	}
	return NULL;
}

static int nftnl_expr_objref_cb(const struct nlattr *attr, void *data)
{
	int type = mnl_attr_get_type(attr);
	const struct nlattr **tb = data;

	if (mnl_attr_type_valid(attr, NFTA_OBJREF_MAX) < 0)
		return MNL_CB_OK;

	switch(type) {
	case NFTA_OBJREF_IMM_TYPE:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	case NFTA_OBJREF_IMM_NAME:
	case NFTA_OBJREF_SET_NAME:
		if (mnl_attr_validate(attr, MNL_TYPE_STRING) < 0)
			abi_breakage();
		break;
	case NFTA_OBJREF_SET_SREG:
	case NFTA_OBJREF_SET_ID:
		if (mnl_attr_validate(attr, MNL_TYPE_U32) < 0)
			abi_breakage();
		break;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}

static void nftnl_expr_objref_build(struct nlmsghdr *nlh,
				    const struct nftnl_expr *e)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_OBJREF_IMM_TYPE))
		mnl_attr_put_u32(nlh, NFTA_OBJREF_IMM_TYPE,
				 htonl(objref->imm.type));
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_IMM_NAME))
		mnl_attr_put_str(nlh, NFTA_OBJREF_IMM_NAME, objref->imm.name);
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_SET_SREG))
		mnl_attr_put_u32(nlh, NFTA_OBJREF_SET_SREG,
				 htonl(objref->set.sreg));
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_SET_NAME))
		mnl_attr_put_str(nlh, NFTA_OBJREF_SET_NAME, objref->set.name);
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_SET_ID))
		mnl_attr_put_u32(nlh, NFTA_OBJREF_SET_ID,
				 htonl(objref->set.id));
}

static int nftnl_expr_objref_parse(struct nftnl_expr *e, struct nlattr *attr)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);
	struct nlattr *tb[NFTA_OBJREF_MAX + 1] = {};

	if (mnl_attr_parse_nested(attr, nftnl_expr_objref_cb, tb) < 0)
		return -1;

	if (tb[NFTA_OBJREF_IMM_TYPE]) {
		objref->imm.type =
			ntohl(mnl_attr_get_u32(tb[NFTA_OBJREF_IMM_TYPE]));
		e->flags |= (1 << NFTNL_EXPR_OBJREF_IMM_TYPE);
	}
	if (tb[NFTA_OBJREF_IMM_NAME]) {
		objref->imm.name =
			strdup(mnl_attr_get_str(tb[NFTA_OBJREF_IMM_NAME]));
		e->flags |= (1 << NFTNL_EXPR_OBJREF_IMM_NAME);
	}
	if (tb[NFTA_OBJREF_SET_SREG]) {
		objref->set.sreg =
			ntohl(mnl_attr_get_u32(tb[NFTA_OBJREF_SET_SREG]));
		e->flags |= (1 << NFTNL_EXPR_OBJREF_SET_SREG);
	}
	if (tb[NFTA_OBJREF_SET_NAME]) {
		objref->set.name =
			strdup(mnl_attr_get_str(tb[NFTA_OBJREF_SET_NAME]));
		e->flags |= (1 << NFTNL_EXPR_OBJREF_SET_NAME);
	}
	if (tb[NFTA_OBJREF_SET_ID]) {
		objref->set.id =
			ntohl(mnl_attr_get_u32(tb[NFTA_OBJREF_SET_ID]));
		e->flags |= (1 << NFTNL_EXPR_OBJREF_SET_ID);
	}

	return 0;
}

static int
nftnl_expr_objref_json_parse(struct nftnl_expr *e, json_t *root,
			     struct nftnl_parse_err *err)
{
#ifdef JSON_PARSING
	uint32_t uval32;
	const char *str;

	if (nftnl_jansson_node_exist(root, "name")) {
		str = nftnl_jansson_parse_str(root, "name", err);
		if (str == NULL)
			return -1;

		nftnl_expr_set_str(e, NFTNL_EXPR_OBJREF_IMM_NAME, str);
	}

	if (nftnl_jansson_parse_val(root, "type", NFTNL_TYPE_U32, &uval32,
				    err) == 0)
		nftnl_expr_set_u32(e, NFTNL_EXPR_OBJREF_IMM_TYPE, uval32);

	return 0;
#else
	errno = EOPNOTSUPP;
	return -1;
#endif
}

static int nftnl_expr_objref_export(char *buf, size_t size,
				    const struct nftnl_expr *e, int type)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);
	NFTNL_BUF_INIT(b, buf, size);

	if (e->flags & (1 << NFTNL_EXPR_OBJREF_IMM_TYPE))
		nftnl_buf_u32(&b, type, objref->imm.type, BYTES);
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_IMM_NAME))
		nftnl_buf_str(&b, type, objref->imm.name, NAME);
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_SET_SREG))
		nftnl_buf_u32(&b, type, objref->set.sreg, SREG);
	if (e->flags & (1 << NFTNL_EXPR_OBJREF_SET_NAME))
		nftnl_buf_str(&b, type, objref->set.name, SET);

	return nftnl_buf_done(&b);
}

static int nftnl_expr_objref_snprintf_default(char *buf, size_t len,
					      const struct nftnl_expr *e)
{
	struct nftnl_expr_objref *objref = nftnl_expr_data(e);

	if (e->flags & (1 << NFTNL_EXPR_OBJREF_SET_SREG))
		return snprintf(buf, len, "sreg %u set %s ",
				objref->set.sreg, objref->set.name);
	else
		return snprintf(buf, len, "type %u name %s ",
				objref->imm.type, objref->imm.name);
}

static int nftnl_expr_objref_snprintf(char *buf, size_t len, uint32_t type,
				      uint32_t flags,
				      const struct nftnl_expr *e)
{
	switch (type) {
	case NFTNL_OUTPUT_DEFAULT:
		return nftnl_expr_objref_snprintf_default(buf, len, e);
	case NFTNL_OUTPUT_XML:
	case NFTNL_OUTPUT_JSON:
		return nftnl_expr_objref_export(buf, len, e, type);
	default:
		break;
	}
	return -1;
}

static bool nftnl_expr_objref_cmp(const struct nftnl_expr *e1,
				   const struct nftnl_expr *e2)
{
	struct nftnl_expr_objref *c1 = nftnl_expr_data(e1);
	struct nftnl_expr_objref *c2 = nftnl_expr_data(e2);
	bool eq = true;

	if (e1->flags & (1 << NFTNL_EXPR_OBJREF_IMM_TYPE))
		eq &= (c1->imm.type == c2->imm.type);
	if (e1->flags & (1 << NFTNL_EXPR_OBJREF_IMM_NAME))
		eq &= !strcmp(c1->imm.name, c2->imm.name);
	if (e1->flags & (1 << NFTNL_EXPR_OBJREF_SET_SREG))
		eq &= (c1->set.sreg == c2->set.sreg);
	if (e1->flags & (1 << NFTNL_EXPR_OBJREF_SET_NAME))
		eq &= !strcmp(c1->set.name, c2->set.name);
	if (e1->flags & (1 << NFTNL_EXPR_OBJREF_SET_ID))
		eq &= (c1->set.id == c2->set.id);

	return eq;
}

struct expr_ops expr_ops_objref = {
	.name		= "objref",
	.alloc_len	= sizeof(struct nftnl_expr_objref),
	.max_attr	= NFTA_OBJREF_MAX,
	.cmp		= nftnl_expr_objref_cmp,
	.set		= nftnl_expr_objref_set,
	.get		= nftnl_expr_objref_get,
	.parse		= nftnl_expr_objref_parse,
	.build		= nftnl_expr_objref_build,
	.snprintf	= nftnl_expr_objref_snprintf,
	.json_parse	= nftnl_expr_objref_json_parse,
};
