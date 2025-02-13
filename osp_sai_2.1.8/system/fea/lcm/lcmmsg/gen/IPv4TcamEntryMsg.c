/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#include <asn_internal.h>

#include "IPv4TcamEntryMsg.h"

static asn_TYPE_member_t asn_MBR_IPv4TcamEntryMsg_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct IPv4TcamEntryMsg, index),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"index"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct IPv4TcamEntryMsg, chipid),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"chipid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct IPv4TcamEntryMsg, keydata),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_IPv4RouteKey,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keydata"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct IPv4TcamEntryMsg, keymask),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_IPv4RouteKey,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keymask"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct IPv4TcamEntryMsg, dsipda),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_DSIpDA,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dsipda"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct IPv4TcamEntryMsg, dsipsa),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_DSIpSA,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dsipsa"
		},
};
static ber_tlv_tag_t asn_DEF_IPv4TcamEntryMsg_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_IPv4TcamEntryMsg_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* index at 902 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* chipid at 903 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* keydata at 904 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* keymask at 905 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* dsipda at 906 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* dsipsa at 908 */
};
static asn_SEQUENCE_specifics_t asn_SPC_IPv4TcamEntryMsg_specs_1 = {
	sizeof(struct IPv4TcamEntryMsg),
	offsetof(struct IPv4TcamEntryMsg, _asn_ctx),
	asn_MAP_IPv4TcamEntryMsg_tag2el_1,
	6,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_IPv4TcamEntryMsg = {
	"IPv4TcamEntryMsg",
	"IPv4TcamEntryMsg",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_IPv4TcamEntryMsg_tags_1,
	sizeof(asn_DEF_IPv4TcamEntryMsg_tags_1)
		/sizeof(asn_DEF_IPv4TcamEntryMsg_tags_1[0]), /* 1 */
	asn_DEF_IPv4TcamEntryMsg_tags_1,	/* Same as above */
	sizeof(asn_DEF_IPv4TcamEntryMsg_tags_1)
		/sizeof(asn_DEF_IPv4TcamEntryMsg_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_IPv4TcamEntryMsg_1,
	6,	/* Elements count */
	&asn_SPC_IPv4TcamEntryMsg_specs_1	/* Additional specs */
};

