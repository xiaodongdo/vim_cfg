/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#include <asn_internal.h>

#include "HalMsgQosSetPortQueueWdrrWeightReq.h"

static asn_TYPE_member_t asn_MBR_weightList_3[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_weightList_tags_3[] = {
	(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_weightList_specs_3 = {
	sizeof(struct weightList),
	offsetof(struct weightList, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_weightList_3 = {
	"weightList",
	"weightList",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_weightList_tags_3,
	sizeof(asn_DEF_weightList_tags_3)
		/sizeof(asn_DEF_weightList_tags_3[0]) - 1, /* 1 */
	asn_DEF_weightList_tags_3,	/* Same as above */
	sizeof(asn_DEF_weightList_tags_3)
		/sizeof(asn_DEF_weightList_tags_3[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_weightList_3,
	1,	/* Single element */
	&asn_SPC_weightList_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_HalMsgQosSetPortQueueWdrrWeightReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgQosSetPortQueueWdrrWeightReq, port),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"port"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgQosSetPortQueueWdrrWeightReq, weightList),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_weightList_3,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"weightList"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgQosSetPortQueueWdrrWeightReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgQosSetPortQueueWdrrWeightReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* port at 329 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* weightList at 331 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgQosSetPortQueueWdrrWeightReq_specs_1 = {
	sizeof(struct HalMsgQosSetPortQueueWdrrWeightReq),
	offsetof(struct HalMsgQosSetPortQueueWdrrWeightReq, _asn_ctx),
	asn_MAP_HalMsgQosSetPortQueueWdrrWeightReq_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgQosSetPortQueueWdrrWeightReq = {
	"HalMsgQosSetPortQueueWdrrWeightReq",
	"HalMsgQosSetPortQueueWdrrWeightReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgQosSetPortQueueWdrrWeightReq_tags_1,
	sizeof(asn_DEF_HalMsgQosSetPortQueueWdrrWeightReq_tags_1)
		/sizeof(asn_DEF_HalMsgQosSetPortQueueWdrrWeightReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgQosSetPortQueueWdrrWeightReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgQosSetPortQueueWdrrWeightReq_tags_1)
		/sizeof(asn_DEF_HalMsgQosSetPortQueueWdrrWeightReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgQosSetPortQueueWdrrWeightReq_1,
	2,	/* Elements count */
	&asn_SPC_HalMsgQosSetPortQueueWdrrWeightReq_specs_1	/* Additional specs */
};

