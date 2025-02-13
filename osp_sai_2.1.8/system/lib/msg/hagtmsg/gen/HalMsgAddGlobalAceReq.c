/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-ACL"
 * 	found in "../aclqos/hal_msg_acl.asn1"
 */

#include <asn_internal.h>

#include "HalMsgAddGlobalAceReq.h"

static asn_TYPE_member_t asn_MBR_HalMsgAddGlobalAceReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgAddGlobalAceReq, enable),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"enable"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgAddGlobalAceReq, seqNum),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"seqNum"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgAddGlobalAceReq, firstOrlast),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"firstOrlast"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgAddGlobalAceReq, mac),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgDefL2MacAddr,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mac"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgAddGlobalAceReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgAddGlobalAceReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* enable at 149 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* seqNum at 150 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* firstOrlast at 151 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* mac at 153 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgAddGlobalAceReq_specs_1 = {
	sizeof(struct HalMsgAddGlobalAceReq),
	offsetof(struct HalMsgAddGlobalAceReq, _asn_ctx),
	asn_MAP_HalMsgAddGlobalAceReq_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgAddGlobalAceReq = {
	"HalMsgAddGlobalAceReq",
	"HalMsgAddGlobalAceReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgAddGlobalAceReq_tags_1,
	sizeof(asn_DEF_HalMsgAddGlobalAceReq_tags_1)
		/sizeof(asn_DEF_HalMsgAddGlobalAceReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgAddGlobalAceReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgAddGlobalAceReq_tags_1)
		/sizeof(asn_DEF_HalMsgAddGlobalAceReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgAddGlobalAceReq_1,
	4,	/* Elements count */
	&asn_SPC_HalMsgAddGlobalAceReq_specs_1	/* Additional specs */
};

