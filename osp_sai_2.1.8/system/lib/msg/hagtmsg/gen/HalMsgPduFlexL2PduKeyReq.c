/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-CPU-TRAFFIC"
 * 	found in "../common/hal_msg_cpu_traffic.asn1"
 */

#include <asn_internal.h>

#include "HalMsgPduFlexL2PduKeyReq.h"

static asn_TYPE_member_t asn_MBR_byMacda_5[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct byMacda, macda),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgDefL2MacAddr,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"macda"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct byMacda, macdaMask),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgDefL2MacAddr,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"macdaMask"
		},
};
static ber_tlv_tag_t asn_DEF_byMacda_tags_5[] = {
	(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_byMacda_tag2el_5[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* macda at 55 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* macdaMask at 57 */
};
static asn_SEQUENCE_specifics_t asn_SPC_byMacda_specs_5 = {
	sizeof(struct byMacda),
	offsetof(struct byMacda, _asn_ctx),
	asn_MAP_byMacda_tag2el_5,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_byMacda_5 = {
	"byMacda",
	"byMacda",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_byMacda_tags_5,
	sizeof(asn_DEF_byMacda_tags_5)
		/sizeof(asn_DEF_byMacda_tags_5[0]) - 1, /* 1 */
	asn_DEF_byMacda_tags_5,	/* Same as above */
	sizeof(asn_DEF_byMacda_tags_5)
		/sizeof(asn_DEF_byMacda_tags_5[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_byMacda_5,
	2,	/* Elements count */
	&asn_SPC_byMacda_specs_5	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_key_4[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct key, byMacda),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_byMacda_5,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"byMacda"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct key, etherType),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"etherType"
		},
};
static asn_TYPE_tag2member_t asn_MAP_key_tag2el_4[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* byMacda at 55 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* etherType at 59 */
};
static asn_CHOICE_specifics_t asn_SPC_key_specs_4 = {
	sizeof(struct key),
	offsetof(struct key, _asn_ctx),
	offsetof(struct key, present),
	sizeof(((struct key *)0)->present),
	asn_MAP_key_tag2el_4,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_key_4 = {
	"key",
	"key",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_key_4,
	2,	/* Elements count */
	&asn_SPC_key_specs_4	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_HalMsgPduFlexL2PduKeyReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgPduFlexL2PduKeyReq, flexL2pdu),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"flexL2pdu"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgPduFlexL2PduKeyReq, keyType),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keyType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgPduFlexL2PduKeyReq, key),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_key_4,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"key"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgPduFlexL2PduKeyReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgPduFlexL2PduKeyReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* flexL2pdu at 49 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* keyType at 50 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* key at 57 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgPduFlexL2PduKeyReq_specs_1 = {
	sizeof(struct HalMsgPduFlexL2PduKeyReq),
	offsetof(struct HalMsgPduFlexL2PduKeyReq, _asn_ctx),
	asn_MAP_HalMsgPduFlexL2PduKeyReq_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgPduFlexL2PduKeyReq = {
	"HalMsgPduFlexL2PduKeyReq",
	"HalMsgPduFlexL2PduKeyReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgPduFlexL2PduKeyReq_tags_1,
	sizeof(asn_DEF_HalMsgPduFlexL2PduKeyReq_tags_1)
		/sizeof(asn_DEF_HalMsgPduFlexL2PduKeyReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgPduFlexL2PduKeyReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgPduFlexL2PduKeyReq_tags_1)
		/sizeof(asn_DEF_HalMsgPduFlexL2PduKeyReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgPduFlexL2PduKeyReq_1,
	3,	/* Elements count */
	&asn_SPC_HalMsgPduFlexL2PduKeyReq_specs_1	/* Additional specs */
};

