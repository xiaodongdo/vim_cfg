/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-PBR"
 * 	found in "../l3/hal_msg_pbr.asn1"
 */

#include <asn_internal.h>

#include "HalMsgPBRUpdatePolicyRmapActionReq.h"

static asn_TYPE_member_t asn_MBR_pbrUpdateEntryIdList_3[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_HalMsgPBREntryId,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_pbrUpdateEntryIdList_tags_3[] = {
	(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_pbrUpdateEntryIdList_specs_3 = {
	sizeof(struct pbrUpdateEntryIdList),
	offsetof(struct pbrUpdateEntryIdList, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_pbrUpdateEntryIdList_3 = {
	"pbrUpdateEntryIdList",
	"pbrUpdateEntryIdList",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_pbrUpdateEntryIdList_tags_3,
	sizeof(asn_DEF_pbrUpdateEntryIdList_tags_3)
		/sizeof(asn_DEF_pbrUpdateEntryIdList_tags_3[0]) - 1, /* 1 */
	asn_DEF_pbrUpdateEntryIdList_tags_3,	/* Same as above */
	sizeof(asn_DEF_pbrUpdateEntryIdList_tags_3)
		/sizeof(asn_DEF_pbrUpdateEntryIdList_tags_3[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_pbrUpdateEntryIdList_3,
	1,	/* Single element */
	&asn_SPC_pbrUpdateEntryIdList_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_HalMsgPBRUpdatePolicyRmapActionReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgPBRUpdatePolicyRmapActionReq, label),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"label"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgPBRUpdatePolicyRmapActionReq, pbrUpdateEntryIdList),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_pbrUpdateEntryIdList_3,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"pbrUpdateEntryIdList"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgPBRUpdatePolicyRmapActionReq, action),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgPBRAction,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"action"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgPBRUpdatePolicyRmapActionReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgPBRUpdatePolicyRmapActionReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* label at 103 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* pbrUpdateEntryIdList at 104 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* action at 106 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgPBRUpdatePolicyRmapActionReq_specs_1 = {
	sizeof(struct HalMsgPBRUpdatePolicyRmapActionReq),
	offsetof(struct HalMsgPBRUpdatePolicyRmapActionReq, _asn_ctx),
	asn_MAP_HalMsgPBRUpdatePolicyRmapActionReq_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgPBRUpdatePolicyRmapActionReq = {
	"HalMsgPBRUpdatePolicyRmapActionReq",
	"HalMsgPBRUpdatePolicyRmapActionReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgPBRUpdatePolicyRmapActionReq_tags_1,
	sizeof(asn_DEF_HalMsgPBRUpdatePolicyRmapActionReq_tags_1)
		/sizeof(asn_DEF_HalMsgPBRUpdatePolicyRmapActionReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgPBRUpdatePolicyRmapActionReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgPBRUpdatePolicyRmapActionReq_tags_1)
		/sizeof(asn_DEF_HalMsgPBRUpdatePolicyRmapActionReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgPBRUpdatePolicyRmapActionReq_1,
	3,	/* Elements count */
	&asn_SPC_HalMsgPBRUpdatePolicyRmapActionReq_specs_1	/* Additional specs */
};

