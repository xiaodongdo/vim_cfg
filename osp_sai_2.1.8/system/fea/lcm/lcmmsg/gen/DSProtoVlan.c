/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#include <asn_internal.h>

#include "DSProtoVlan.h"

static asn_TYPE_member_t asn_MBR_DSProtoVlan_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct DSProtoVlan, protoVlanidValid),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"protoVlanidValid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSProtoVlan, cpuExcpEn),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"cpuExcpEn"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSProtoVlan, discard),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"discard"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSProtoVlan, replaceTagEn),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"replaceTagEn"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSProtoVlan, ptlvid),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ptlvid"
		},
};
static ber_tlv_tag_t asn_DEF_DSProtoVlan_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_DSProtoVlan_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* protoVlanidValid at 562 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* cpuExcpEn at 563 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* discard at 564 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* replaceTagEn at 565 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 } /* ptlvid at 567 */
};
static asn_SEQUENCE_specifics_t asn_SPC_DSProtoVlan_specs_1 = {
	sizeof(struct DSProtoVlan),
	offsetof(struct DSProtoVlan, _asn_ctx),
	asn_MAP_DSProtoVlan_tag2el_1,
	5,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_DSProtoVlan = {
	"DSProtoVlan",
	"DSProtoVlan",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_DSProtoVlan_tags_1,
	sizeof(asn_DEF_DSProtoVlan_tags_1)
		/sizeof(asn_DEF_DSProtoVlan_tags_1[0]), /* 1 */
	asn_DEF_DSProtoVlan_tags_1,	/* Same as above */
	sizeof(asn_DEF_DSProtoVlan_tags_1)
		/sizeof(asn_DEF_DSProtoVlan_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_DSProtoVlan_1,
	5,	/* Elements count */
	&asn_SPC_DSProtoVlan_specs_1	/* Additional specs */
};

