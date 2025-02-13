/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-FLOW"
 * 	found in "../openflow/hal_msg_openflow_flow.asn1"
 */

#include <asn_internal.h>

#include "HalMsgOpenFlowRule.h"

static asn_TYPE_member_t asn_MBR_unionRule_5[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct unionRule, arpRule),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgOpenFlowArpRule,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"arpRule"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct unionRule, ipv4Rule),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgOpenFlowIpv4Rule,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipv4Rule"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct unionRule, ipv6Rule),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgOpenFlowIpv6Rule,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipv6Rule"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct unionRule, mplsRule),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgOpenFlowMplsRule,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mplsRule"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct unionRule, mplsMcastRule),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgOpenFlowMplsRule,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mplsMcastRule"
		},
};
static asn_TYPE_tag2member_t asn_MAP_unionRule_tag2el_5[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* arpRule at 154 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* ipv4Rule at 155 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* ipv6Rule at 156 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* mplsRule at 157 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 } /* mplsMcastRule at 159 */
};
static asn_CHOICE_specifics_t asn_SPC_unionRule_specs_5 = {
	sizeof(struct unionRule),
	offsetof(struct unionRule, _asn_ctx),
	offsetof(struct unionRule, present),
	sizeof(((struct unionRule *)0)->present),
	asn_MAP_unionRule_tag2el_5,
	5,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_unionRule_5 = {
	"unionRule",
	"unionRule",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_unionRule_5,
	5,	/* Elements count */
	&asn_SPC_unionRule_specs_5	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_HalMsgOpenFlowRule_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowRule, macRule),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgOpenFlowMacRule,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"macRule"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowRule, tnlRule),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgOpenFlowTnlRule,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tnlRule"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowRule, udfRule),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgOpenFlowUdfRule,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"udfRule"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowRule, unionRule),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_unionRule_5,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"unionRule"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgOpenFlowRule_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgOpenFlowRule_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* macRule at 148 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* tnlRule at 149 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* udfRule at 150 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* unionRule at 154 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgOpenFlowRule_specs_1 = {
	sizeof(struct HalMsgOpenFlowRule),
	offsetof(struct HalMsgOpenFlowRule, _asn_ctx),
	asn_MAP_HalMsgOpenFlowRule_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowRule = {
	"HalMsgOpenFlowRule",
	"HalMsgOpenFlowRule",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgOpenFlowRule_tags_1,
	sizeof(asn_DEF_HalMsgOpenFlowRule_tags_1)
		/sizeof(asn_DEF_HalMsgOpenFlowRule_tags_1[0]), /* 1 */
	asn_DEF_HalMsgOpenFlowRule_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgOpenFlowRule_tags_1)
		/sizeof(asn_DEF_HalMsgOpenFlowRule_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgOpenFlowRule_1,
	4,	/* Elements count */
	&asn_SPC_HalMsgOpenFlowRule_specs_1	/* Additional specs */
};

