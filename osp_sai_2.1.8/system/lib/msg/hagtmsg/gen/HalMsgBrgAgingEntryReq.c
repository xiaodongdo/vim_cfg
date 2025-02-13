/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L2-BRG"
 * 	found in "../l2/hal_msg_bridge.asn1"
 */

#include <asn_internal.h>

#include "HalMsgBrgAgingEntryReq.h"

static asn_TYPE_member_t asn_MBR_HalMsgBrgAgingEntryReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgBrgAgingEntryReq, mac),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgDefL2MacAddr,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mac"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgBrgAgingEntryReq, fid),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"fid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgBrgAgingEntryReq, glSrcPort),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"glSrcPort"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgBrgAgingEntryReq, gsrcportType),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"gsrcportType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgBrgAgingEntryReq, label),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"label"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgBrgAgingEntryReq, nhId),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"nhId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgBrgAgingEntryReq, flag),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"flag"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgBrgAgingEntryReq, virtualPort),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"virtualPort"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgBrgAgingEntryReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgBrgAgingEntryReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* mac at 143 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* fid at 144 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* glSrcPort at 145 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* gsrcportType at 146 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* label at 147 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* nhId at 148 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* flag at 149 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 } /* virtualPort at 151 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgBrgAgingEntryReq_specs_1 = {
	sizeof(struct HalMsgBrgAgingEntryReq),
	offsetof(struct HalMsgBrgAgingEntryReq, _asn_ctx),
	asn_MAP_HalMsgBrgAgingEntryReq_tag2el_1,
	8,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgBrgAgingEntryReq = {
	"HalMsgBrgAgingEntryReq",
	"HalMsgBrgAgingEntryReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgBrgAgingEntryReq_tags_1,
	sizeof(asn_DEF_HalMsgBrgAgingEntryReq_tags_1)
		/sizeof(asn_DEF_HalMsgBrgAgingEntryReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgBrgAgingEntryReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgBrgAgingEntryReq_tags_1)
		/sizeof(asn_DEF_HalMsgBrgAgingEntryReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgBrgAgingEntryReq_1,
	8,	/* Elements count */
	&asn_SPC_HalMsgBrgAgingEntryReq_specs_1	/* Additional specs */
};

