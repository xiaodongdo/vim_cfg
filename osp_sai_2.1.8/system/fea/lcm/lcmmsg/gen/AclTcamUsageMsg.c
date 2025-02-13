/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#include <asn_internal.h>

#include "AclTcamUsageMsg.h"

static asn_TYPE_member_t asn_MBR_AclTcamUsageMsg_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct AclTcamUsageMsg, macIpv4Total),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"macIpv4Total"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct AclTcamUsageMsg, macIpv4Used),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"macIpv4Used"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct AclTcamUsageMsg, ipv6Total),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipv6Total"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct AclTcamUsageMsg, ipv6Used),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipv6Used"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct AclTcamUsageMsg, labelTotal),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"labelTotal"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct AclTcamUsageMsg, labelUsed),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"labelUsed"
		},
};
static ber_tlv_tag_t asn_DEF_AclTcamUsageMsg_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_AclTcamUsageMsg_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* macIpv4Total at 42 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* macIpv4Used at 43 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* ipv6Total at 44 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* ipv6Used at 45 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* labelTotal at 46 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* labelUsed at 48 */
};
static asn_SEQUENCE_specifics_t asn_SPC_AclTcamUsageMsg_specs_1 = {
	sizeof(struct AclTcamUsageMsg),
	offsetof(struct AclTcamUsageMsg, _asn_ctx),
	asn_MAP_AclTcamUsageMsg_tag2el_1,
	6,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_AclTcamUsageMsg = {
	"AclTcamUsageMsg",
	"AclTcamUsageMsg",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_AclTcamUsageMsg_tags_1,
	sizeof(asn_DEF_AclTcamUsageMsg_tags_1)
		/sizeof(asn_DEF_AclTcamUsageMsg_tags_1[0]), /* 1 */
	asn_DEF_AclTcamUsageMsg_tags_1,	/* Same as above */
	sizeof(asn_DEF_AclTcamUsageMsg_tags_1)
		/sizeof(asn_DEF_AclTcamUsageMsg_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_AclTcamUsageMsg_1,
	6,	/* Elements count */
	&asn_SPC_AclTcamUsageMsg_specs_1	/* Additional specs */
};

