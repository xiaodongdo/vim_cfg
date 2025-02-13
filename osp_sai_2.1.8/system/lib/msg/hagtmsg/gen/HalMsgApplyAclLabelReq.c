/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-ACL"
 * 	found in "../aclqos/hal_msg_acl.asn1"
 */

#include <asn_internal.h>

#include "HalMsgApplyAclLabelReq.h"

static asn_TYPE_member_t asn_MBR_HalMsgApplyAclLabelReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgApplyAclLabelReq, isVlan),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"isVlan"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgApplyAclLabelReq, portOrVid),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"portOrVid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgApplyAclLabelReq, dir),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dir"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgApplyAclLabelReq, label),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"label"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgApplyAclLabelReq, isGlobalAcl),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"isGlobalAcl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgApplyAclLabelReq, isFirstMemner),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"isFirstMemner"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgApplyAclLabelReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgApplyAclLabelReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* isVlan at 62 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* portOrVid at 63 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* dir at 64 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* label at 65 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* isGlobalAcl at 66 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* isFirstMemner at 68 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgApplyAclLabelReq_specs_1 = {
	sizeof(struct HalMsgApplyAclLabelReq),
	offsetof(struct HalMsgApplyAclLabelReq, _asn_ctx),
	asn_MAP_HalMsgApplyAclLabelReq_tag2el_1,
	6,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgApplyAclLabelReq = {
	"HalMsgApplyAclLabelReq",
	"HalMsgApplyAclLabelReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgApplyAclLabelReq_tags_1,
	sizeof(asn_DEF_HalMsgApplyAclLabelReq_tags_1)
		/sizeof(asn_DEF_HalMsgApplyAclLabelReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgApplyAclLabelReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgApplyAclLabelReq_tags_1)
		/sizeof(asn_DEF_HalMsgApplyAclLabelReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgApplyAclLabelReq_1,
	6,	/* Elements count */
	&asn_SPC_HalMsgApplyAclLabelReq_specs_1	/* Additional specs */
};

