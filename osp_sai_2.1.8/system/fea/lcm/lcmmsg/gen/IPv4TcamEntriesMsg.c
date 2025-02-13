/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#include <asn_internal.h>

#include "IPv4TcamEntriesMsg.h"

static asn_TYPE_member_t asn_MBR_ipv4TcamEntries_4[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_IPv4TcamEntryMsg,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_ipv4TcamEntries_tags_4[] = {
	(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_ipv4TcamEntries_specs_4 = {
	sizeof(struct ipv4TcamEntries),
	offsetof(struct ipv4TcamEntries, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_ipv4TcamEntries_4 = {
	"ipv4TcamEntries",
	"ipv4TcamEntries",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ipv4TcamEntries_tags_4,
	sizeof(asn_DEF_ipv4TcamEntries_tags_4)
		/sizeof(asn_DEF_ipv4TcamEntries_tags_4[0]) - 1, /* 1 */
	asn_DEF_ipv4TcamEntries_tags_4,	/* Same as above */
	sizeof(asn_DEF_ipv4TcamEntries_tags_4)
		/sizeof(asn_DEF_ipv4TcamEntries_tags_4[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_ipv4TcamEntries_4,
	1,	/* Single element */
	&asn_SPC_ipv4TcamEntries_specs_4	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_IPv4TcamEntriesMsg_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct IPv4TcamEntriesMsg, entryNumThisTime),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"entryNumThisTime"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct IPv4TcamEntriesMsg, bitmapUpto),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"bitmapUpto"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct IPv4TcamEntriesMsg, ipv4TcamEntries),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ipv4TcamEntries_4,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipv4TcamEntries"
		},
};
static ber_tlv_tag_t asn_DEF_IPv4TcamEntriesMsg_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_IPv4TcamEntriesMsg_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* entryNumThisTime at 973 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* bitmapUpto at 974 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* ipv4TcamEntries at 976 */
};
static asn_SEQUENCE_specifics_t asn_SPC_IPv4TcamEntriesMsg_specs_1 = {
	sizeof(struct IPv4TcamEntriesMsg),
	offsetof(struct IPv4TcamEntriesMsg, _asn_ctx),
	asn_MAP_IPv4TcamEntriesMsg_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_IPv4TcamEntriesMsg = {
	"IPv4TcamEntriesMsg",
	"IPv4TcamEntriesMsg",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_IPv4TcamEntriesMsg_tags_1,
	sizeof(asn_DEF_IPv4TcamEntriesMsg_tags_1)
		/sizeof(asn_DEF_IPv4TcamEntriesMsg_tags_1[0]), /* 1 */
	asn_DEF_IPv4TcamEntriesMsg_tags_1,	/* Same as above */
	sizeof(asn_DEF_IPv4TcamEntriesMsg_tags_1)
		/sizeof(asn_DEF_IPv4TcamEntriesMsg_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_IPv4TcamEntriesMsg_1,
	3,	/* Elements count */
	&asn_SPC_IPv4TcamEntriesMsg_specs_1	/* Additional specs */
};

