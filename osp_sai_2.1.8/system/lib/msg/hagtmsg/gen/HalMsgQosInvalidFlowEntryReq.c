/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#include <asn_internal.h>

#include "HalMsgQosInvalidFlowEntryReq.h"

static asn_TYPE_member_t asn_MBR_HalMsgQosInvalidFlowEntryReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgQosInvalidFlowEntryReq, label),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"label"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgQosInvalidFlowEntryReq, entryId),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgQosFlowEntryId,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"entryId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgQosInvalidFlowEntryReq, invalid),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"invalid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgQosInvalidFlowEntryReq, rule),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgFlowRule,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"rule"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgQosInvalidFlowEntryReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgQosInvalidFlowEntryReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* label at 230 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* entryId at 231 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* invalid at 232 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* rule at 234 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgQosInvalidFlowEntryReq_specs_1 = {
	sizeof(struct HalMsgQosInvalidFlowEntryReq),
	offsetof(struct HalMsgQosInvalidFlowEntryReq, _asn_ctx),
	asn_MAP_HalMsgQosInvalidFlowEntryReq_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgQosInvalidFlowEntryReq = {
	"HalMsgQosInvalidFlowEntryReq",
	"HalMsgQosInvalidFlowEntryReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgQosInvalidFlowEntryReq_tags_1,
	sizeof(asn_DEF_HalMsgQosInvalidFlowEntryReq_tags_1)
		/sizeof(asn_DEF_HalMsgQosInvalidFlowEntryReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgQosInvalidFlowEntryReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgQosInvalidFlowEntryReq_tags_1)
		/sizeof(asn_DEF_HalMsgQosInvalidFlowEntryReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgQosInvalidFlowEntryReq_1,
	4,	/* Elements count */
	&asn_SPC_HalMsgQosInvalidFlowEntryReq_specs_1	/* Additional specs */
};

