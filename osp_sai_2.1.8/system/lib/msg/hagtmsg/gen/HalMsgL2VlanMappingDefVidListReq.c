/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L2-VLAN-MAPPING"
 * 	found in "../l2/hal_msg_vlan_mapping.asn1"
 */

#include <asn_internal.h>

#include "HalMsgL2VlanMappingDefVidListReq.h"

static asn_TYPE_member_t asn_MBR_vlanMappingDefVidlistEntry_2[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_HalMsgL2VlanMappingDefVidReq,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_vlanMappingDefVidlistEntry_tags_2[] = {
	(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_vlanMappingDefVidlistEntry_specs_2 = {
	sizeof(struct vlanMappingDefVidlistEntry),
	offsetof(struct vlanMappingDefVidlistEntry, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_vlanMappingDefVidlistEntry_2 = {
	"vlanMappingDefVidlistEntry",
	"vlanMappingDefVidlistEntry",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_vlanMappingDefVidlistEntry_tags_2,
	sizeof(asn_DEF_vlanMappingDefVidlistEntry_tags_2)
		/sizeof(asn_DEF_vlanMappingDefVidlistEntry_tags_2[0]) - 1, /* 1 */
	asn_DEF_vlanMappingDefVidlistEntry_tags_2,	/* Same as above */
	sizeof(asn_DEF_vlanMappingDefVidlistEntry_tags_2)
		/sizeof(asn_DEF_vlanMappingDefVidlistEntry_tags_2[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_vlanMappingDefVidlistEntry_2,
	1,	/* Single element */
	&asn_SPC_vlanMappingDefVidlistEntry_specs_2	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_HalMsgL2VlanMappingDefVidListReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2VlanMappingDefVidListReq, vlanMappingDefVidlistEntry),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_vlanMappingDefVidlistEntry_2,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"vlanMappingDefVidlistEntry"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgL2VlanMappingDefVidListReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgL2VlanMappingDefVidListReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 } /* vlanMappingDefVidlistEntry at 70 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgL2VlanMappingDefVidListReq_specs_1 = {
	sizeof(struct HalMsgL2VlanMappingDefVidListReq),
	offsetof(struct HalMsgL2VlanMappingDefVidListReq, _asn_ctx),
	asn_MAP_HalMsgL2VlanMappingDefVidListReq_tag2el_1,
	1,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgL2VlanMappingDefVidListReq = {
	"HalMsgL2VlanMappingDefVidListReq",
	"HalMsgL2VlanMappingDefVidListReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgL2VlanMappingDefVidListReq_tags_1,
	sizeof(asn_DEF_HalMsgL2VlanMappingDefVidListReq_tags_1)
		/sizeof(asn_DEF_HalMsgL2VlanMappingDefVidListReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgL2VlanMappingDefVidListReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgL2VlanMappingDefVidListReq_tags_1)
		/sizeof(asn_DEF_HalMsgL2VlanMappingDefVidListReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgL2VlanMappingDefVidListReq_1,
	1,	/* Elements count */
	&asn_SPC_HalMsgL2VlanMappingDefVidListReq_specs_1	/* Additional specs */
};

