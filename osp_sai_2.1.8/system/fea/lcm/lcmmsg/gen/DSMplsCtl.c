/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#include <asn_internal.h>

#include "DSMplsCtl.h"

static asn_TYPE_member_t asn_MBR_DSMplsCtl_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct DSMplsCtl, numofLabelsMcast),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"numofLabelsMcast"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSMplsCtl, numofLabels),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"numofLabels"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSMplsCtl, labelSpaceSizeTypeMcast),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"labelSpaceSizeTypeMcast"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSMplsCtl, labelSpaceSizeType),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"labelSpaceSizeType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSMplsCtl, labelBaseMcast),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"labelBaseMcast"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSMplsCtl, labelBase),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"labelBase"
		},
};
static ber_tlv_tag_t asn_DEF_DSMplsCtl_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_DSMplsCtl_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* numofLabelsMcast at 800 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* numofLabels at 801 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* labelSpaceSizeTypeMcast at 802 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* labelSpaceSizeType at 803 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* labelBaseMcast at 804 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* labelBase at 806 */
};
static asn_SEQUENCE_specifics_t asn_SPC_DSMplsCtl_specs_1 = {
	sizeof(struct DSMplsCtl),
	offsetof(struct DSMplsCtl, _asn_ctx),
	asn_MAP_DSMplsCtl_tag2el_1,
	6,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_DSMplsCtl = {
	"DSMplsCtl",
	"DSMplsCtl",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_DSMplsCtl_tags_1,
	sizeof(asn_DEF_DSMplsCtl_tags_1)
		/sizeof(asn_DEF_DSMplsCtl_tags_1[0]), /* 1 */
	asn_DEF_DSMplsCtl_tags_1,	/* Same as above */
	sizeof(asn_DEF_DSMplsCtl_tags_1)
		/sizeof(asn_DEF_DSMplsCtl_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_DSMplsCtl_1,
	6,	/* Elements count */
	&asn_SPC_DSMplsCtl_specs_1	/* Additional specs */
};

