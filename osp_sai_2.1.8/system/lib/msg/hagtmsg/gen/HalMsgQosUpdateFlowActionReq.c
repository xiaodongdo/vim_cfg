/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#include <asn_internal.h>

#include "HalMsgQosUpdateFlowActionReq.h"

static asn_TYPE_member_t asn_MBR_updateEntryIdList_3[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_HalMsgQosFlowEntryId,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_updateEntryIdList_tags_3[] = {
	(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_updateEntryIdList_specs_3 = {
	sizeof(struct updateEntryIdList),
	offsetof(struct updateEntryIdList, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_updateEntryIdList_3 = {
	"updateEntryIdList",
	"updateEntryIdList",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_updateEntryIdList_tags_3,
	sizeof(asn_DEF_updateEntryIdList_tags_3)
		/sizeof(asn_DEF_updateEntryIdList_tags_3[0]) - 1, /* 1 */
	asn_DEF_updateEntryIdList_tags_3,	/* Same as above */
	sizeof(asn_DEF_updateEntryIdList_tags_3)
		/sizeof(asn_DEF_updateEntryIdList_tags_3[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_updateEntryIdList_3,
	1,	/* Single element */
	&asn_SPC_updateEntryIdList_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_HalMsgQosUpdateFlowActionReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgQosUpdateFlowActionReq, label),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"label"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgQosUpdateFlowActionReq, updateEntryIdList),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_updateEntryIdList_3,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"updateEntryIdList"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgQosUpdateFlowActionReq, opAction),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"opAction"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgQosUpdateFlowActionReq, action),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgQosFlowAction,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"action"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgQosUpdateFlowActionReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgQosUpdateFlowActionReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* label at 263 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* updateEntryIdList at 264 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* opAction at 265 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* action at 267 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgQosUpdateFlowActionReq_specs_1 = {
	sizeof(struct HalMsgQosUpdateFlowActionReq),
	offsetof(struct HalMsgQosUpdateFlowActionReq, _asn_ctx),
	asn_MAP_HalMsgQosUpdateFlowActionReq_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgQosUpdateFlowActionReq = {
	"HalMsgQosUpdateFlowActionReq",
	"HalMsgQosUpdateFlowActionReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgQosUpdateFlowActionReq_tags_1,
	sizeof(asn_DEF_HalMsgQosUpdateFlowActionReq_tags_1)
		/sizeof(asn_DEF_HalMsgQosUpdateFlowActionReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgQosUpdateFlowActionReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgQosUpdateFlowActionReq_tags_1)
		/sizeof(asn_DEF_HalMsgQosUpdateFlowActionReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgQosUpdateFlowActionReq_1,
	4,	/* Elements count */
	&asn_SPC_HalMsgQosUpdateFlowActionReq_specs_1	/* Additional specs */
};

