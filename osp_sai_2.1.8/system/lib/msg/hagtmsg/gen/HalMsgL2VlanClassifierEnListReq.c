/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L2-VLAN-CLASSIFIER"
 * 	found in "../l2/hal_msg_vlan_classifier.asn1"
 */

#include <asn_internal.h>

#include "HalMsgL2VlanClassifierEnListReq.h"

static asn_TYPE_member_t asn_MBR_listEn_2[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_HalMsgL2VlanClassifierEnReq,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_listEn_tags_2[] = {
	(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_listEn_specs_2 = {
	sizeof(struct listEn),
	offsetof(struct listEn, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_listEn_2 = {
	"listEn",
	"listEn",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_listEn_tags_2,
	sizeof(asn_DEF_listEn_tags_2)
		/sizeof(asn_DEF_listEn_tags_2[0]) - 1, /* 1 */
	asn_DEF_listEn_tags_2,	/* Same as above */
	sizeof(asn_DEF_listEn_tags_2)
		/sizeof(asn_DEF_listEn_tags_2[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_listEn_2,
	1,	/* Single element */
	&asn_SPC_listEn_specs_2	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_HalMsgL2VlanClassifierEnListReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2VlanClassifierEnListReq, listEn),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_listEn_2,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"listEn"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgL2VlanClassifierEnListReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgL2VlanClassifierEnListReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 } /* listEn at 71 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgL2VlanClassifierEnListReq_specs_1 = {
	sizeof(struct HalMsgL2VlanClassifierEnListReq),
	offsetof(struct HalMsgL2VlanClassifierEnListReq, _asn_ctx),
	asn_MAP_HalMsgL2VlanClassifierEnListReq_tag2el_1,
	1,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgL2VlanClassifierEnListReq = {
	"HalMsgL2VlanClassifierEnListReq",
	"HalMsgL2VlanClassifierEnListReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgL2VlanClassifierEnListReq_tags_1,
	sizeof(asn_DEF_HalMsgL2VlanClassifierEnListReq_tags_1)
		/sizeof(asn_DEF_HalMsgL2VlanClassifierEnListReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgL2VlanClassifierEnListReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgL2VlanClassifierEnListReq_tags_1)
		/sizeof(asn_DEF_HalMsgL2VlanClassifierEnListReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgL2VlanClassifierEnListReq_1,
	1,	/* Elements count */
	&asn_SPC_HalMsgL2VlanClassifierEnListReq_specs_1	/* Additional specs */
};

