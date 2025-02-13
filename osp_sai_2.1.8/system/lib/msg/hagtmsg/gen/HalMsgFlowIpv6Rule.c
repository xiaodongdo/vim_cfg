/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-FLOW"
 * 	found in "../aclqos/hal_msg_flow.asn1"
 */

#include <asn_internal.h>

#include "HalMsgFlowIpv6Rule.h"

static asn_TYPE_member_t asn_MBR_HalMsgFlowIpv6Rule_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, flag),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"flag"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, sprefix),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgIpv6addr,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"sprefix"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, sprefixLen),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"sprefixLen"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, dprefix),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgIpv6addr,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dprefix"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, dprefixLen),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dprefixLen"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, l4SrcPort),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgFlowL4PortType,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l4SrcPort"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, l4DestPort),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgFlowL4PortType,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l4DestPort"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, ipFrag),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipFrag"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, tcpFlag),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgFlowTcpFlagType,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tcpFlag"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, ipOptions),
		(ASN_TAG_CLASS_CONTEXT | (9 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipOptions"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, routedPacket),
		(ASN_TAG_CLASS_CONTEXT | (10 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"routedPacket"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, l4Protocol),
		(ASN_TAG_CLASS_CONTEXT | (11 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l4Protocol"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, icmpType),
		(ASN_TAG_CLASS_CONTEXT | (12 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"icmpType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, icmpCode),
		(ASN_TAG_CLASS_CONTEXT | (13 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"icmpCode"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, dscp),
		(ASN_TAG_CLASS_CONTEXT | (14 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dscp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgFlowIpv6Rule, flowLabel),
		(ASN_TAG_CLASS_CONTEXT | (15 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"flowLabel"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgFlowIpv6Rule_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgFlowIpv6Rule_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* flag at 110 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* sprefix at 111 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* sprefixLen at 112 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* dprefix at 113 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* dprefixLen at 114 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* l4SrcPort at 115 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* l4DestPort at 116 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* ipFrag at 117 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 }, /* tcpFlag at 118 */
    { (ASN_TAG_CLASS_CONTEXT | (9 << 2)), 9, 0, 0 }, /* ipOptions at 119 */
    { (ASN_TAG_CLASS_CONTEXT | (10 << 2)), 10, 0, 0 }, /* routedPacket at 120 */
    { (ASN_TAG_CLASS_CONTEXT | (11 << 2)), 11, 0, 0 }, /* l4Protocol at 121 */
    { (ASN_TAG_CLASS_CONTEXT | (12 << 2)), 12, 0, 0 }, /* icmpType at 122 */
    { (ASN_TAG_CLASS_CONTEXT | (13 << 2)), 13, 0, 0 }, /* icmpCode at 123 */
    { (ASN_TAG_CLASS_CONTEXT | (14 << 2)), 14, 0, 0 }, /* dscp at 124 */
    { (ASN_TAG_CLASS_CONTEXT | (15 << 2)), 15, 0, 0 } /* flowLabel at 126 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgFlowIpv6Rule_specs_1 = {
	sizeof(struct HalMsgFlowIpv6Rule),
	offsetof(struct HalMsgFlowIpv6Rule, _asn_ctx),
	asn_MAP_HalMsgFlowIpv6Rule_tag2el_1,
	16,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgFlowIpv6Rule = {
	"HalMsgFlowIpv6Rule",
	"HalMsgFlowIpv6Rule",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgFlowIpv6Rule_tags_1,
	sizeof(asn_DEF_HalMsgFlowIpv6Rule_tags_1)
		/sizeof(asn_DEF_HalMsgFlowIpv6Rule_tags_1[0]), /* 1 */
	asn_DEF_HalMsgFlowIpv6Rule_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgFlowIpv6Rule_tags_1)
		/sizeof(asn_DEF_HalMsgFlowIpv6Rule_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgFlowIpv6Rule_1,
	16,	/* Elements count */
	&asn_SPC_HalMsgFlowIpv6Rule_specs_1	/* Additional specs */
};

