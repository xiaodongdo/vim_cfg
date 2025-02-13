/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-ACL"
 * 	found in "../aclqos/hal_msg_acl.asn1"
 */

#include <asn_internal.h>

#include "HalMsgSetAclEnableReq.h"

static asn_TYPE_member_t asn_MBR_HalMsgSetAclEnableReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgSetAclEnableReq, isVlan),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"isVlan"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgSetAclEnableReq, portOrVid),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"portOrVid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgSetAclEnableReq, dir),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dir"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgSetAclEnableReq, enable),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"enable"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgSetAclEnableReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgSetAclEnableReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* isVlan at 108 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* portOrVid at 109 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* dir at 110 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* enable at 111 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgSetAclEnableReq_specs_1 = {
	sizeof(struct HalMsgSetAclEnableReq),
	offsetof(struct HalMsgSetAclEnableReq, _asn_ctx),
	asn_MAP_HalMsgSetAclEnableReq_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgSetAclEnableReq = {
	"HalMsgSetAclEnableReq",
	"HalMsgSetAclEnableReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgSetAclEnableReq_tags_1,
	sizeof(asn_DEF_HalMsgSetAclEnableReq_tags_1)
		/sizeof(asn_DEF_HalMsgSetAclEnableReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgSetAclEnableReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgSetAclEnableReq_tags_1)
		/sizeof(asn_DEF_HalMsgSetAclEnableReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgSetAclEnableReq_1,
	4,	/* Elements count */
	&asn_SPC_HalMsgSetAclEnableReq_specs_1	/* Additional specs */
};

