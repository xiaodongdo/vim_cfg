/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#include <asn_internal.h>

#include "VlanClassTcamEntrisMsg.h"

static asn_TYPE_member_t asn_MBR_tcamEntry_9[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_VlanClassTcamEntry,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_tcamEntry_tags_9[] = {
	(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_tcamEntry_specs_9 = {
	sizeof(struct tcamEntry),
	offsetof(struct tcamEntry, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_tcamEntry_9 = {
	"tcamEntry",
	"tcamEntry",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_tcamEntry_tags_9,
	sizeof(asn_DEF_tcamEntry_tags_9)
		/sizeof(asn_DEF_tcamEntry_tags_9[0]) - 1, /* 1 */
	asn_DEF_tcamEntry_tags_9,	/* Same as above */
	sizeof(asn_DEF_tcamEntry_tags_9)
		/sizeof(asn_DEF_tcamEntry_tags_9[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_tcamEntry_9,
	1,	/* Single element */
	&asn_SPC_tcamEntry_specs_9	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_VlanClassTcamEntrisMsg_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct VlanClassTcamEntrisMsg, chipid),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"chipid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct VlanClassTcamEntrisMsg, index),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"index"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct VlanClassTcamEntrisMsg, validinfo),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"validinfo"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct VlanClassTcamEntrisMsg, keytype),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keytype"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct VlanClassTcamEntrisMsg, entryNumThisTime),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"entryNumThisTime"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct VlanClassTcamEntrisMsg, entryNumSent),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"entryNumSent"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct VlanClassTcamEntrisMsg, bitmapUpto),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"bitmapUpto"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct VlanClassTcamEntrisMsg, tcamEntry),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_tcamEntry_9,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tcamEntry"
		},
};
static ber_tlv_tag_t asn_DEF_VlanClassTcamEntrisMsg_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_VlanClassTcamEntrisMsg_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* chipid at 1141 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* index at 1142 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* validinfo at 1143 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* keytype at 1144 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* entryNumThisTime at 1145 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* entryNumSent at 1146 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* bitmapUpto at 1147 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 } /* tcamEntry at 1149 */
};
static asn_SEQUENCE_specifics_t asn_SPC_VlanClassTcamEntrisMsg_specs_1 = {
	sizeof(struct VlanClassTcamEntrisMsg),
	offsetof(struct VlanClassTcamEntrisMsg, _asn_ctx),
	asn_MAP_VlanClassTcamEntrisMsg_tag2el_1,
	8,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_VlanClassTcamEntrisMsg = {
	"VlanClassTcamEntrisMsg",
	"VlanClassTcamEntrisMsg",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_VlanClassTcamEntrisMsg_tags_1,
	sizeof(asn_DEF_VlanClassTcamEntrisMsg_tags_1)
		/sizeof(asn_DEF_VlanClassTcamEntrisMsg_tags_1[0]), /* 1 */
	asn_DEF_VlanClassTcamEntrisMsg_tags_1,	/* Same as above */
	sizeof(asn_DEF_VlanClassTcamEntrisMsg_tags_1)
		/sizeof(asn_DEF_VlanClassTcamEntrisMsg_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_VlanClassTcamEntrisMsg_1,
	8,	/* Elements count */
	&asn_SPC_VlanClassTcamEntrisMsg_specs_1	/* Additional specs */
};

