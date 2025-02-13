/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#include <asn_internal.h>

#include "DSL3EditTunnelV4.h"

static asn_TYPE_member_t asn_MBR_DSL3EditTunnelV4_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, innerHeaderType),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"innerHeaderType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, greFlagsMipTypeUp),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"greFlagsMipTypeUp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, greVerMipTypeLower),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"greVerMipTypeLower"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, dscp),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dscp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, ttl),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ttl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, ipProtoType),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipProtoType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, isAtpTunnel),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"isAtpTunnel"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, ipIdentificType),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipIdentificType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, mapDscp),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mapDscp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, mapTtl),
		(ASN_TAG_CLASS_CONTEXT | (9 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mapTtl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, copyDontFrag),
		(ASN_TAG_CLASS_CONTEXT | (10 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"copyDontFrag"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, dontFrag),
		(ASN_TAG_CLASS_CONTEXT | (11 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dontFrag"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, innerHeaderValid),
		(ASN_TAG_CLASS_CONTEXT | (12 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"innerHeaderValid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, tunnel6to4),
		(ASN_TAG_CLASS_CONTEXT | (13 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tunnel6to4"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, tunnelSa6to4),
		(ASN_TAG_CLASS_CONTEXT | (14 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tunnelSa6to4"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, greKeyUdpDestPort),
		(ASN_TAG_CLASS_CONTEXT | (15 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"greKeyUdpDestPort"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, greProtoUdpSrcPort),
		(ASN_TAG_CLASS_CONTEXT | (16 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"greProtoUdpSrcPort"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, ipsa),
		(ASN_TAG_CLASS_CONTEXT | (17 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipsa"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSL3EditTunnelV4, ipda),
		(ASN_TAG_CLASS_CONTEXT | (18 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipda"
		},
};
static ber_tlv_tag_t asn_DEF_DSL3EditTunnelV4_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_DSL3EditTunnelV4_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* innerHeaderType at 684 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* greFlagsMipTypeUp at 685 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* greVerMipTypeLower at 686 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* dscp at 687 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* ttl at 688 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* ipProtoType at 689 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* isAtpTunnel at 690 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* ipIdentificType at 691 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 }, /* mapDscp at 692 */
    { (ASN_TAG_CLASS_CONTEXT | (9 << 2)), 9, 0, 0 }, /* mapTtl at 693 */
    { (ASN_TAG_CLASS_CONTEXT | (10 << 2)), 10, 0, 0 }, /* copyDontFrag at 694 */
    { (ASN_TAG_CLASS_CONTEXT | (11 << 2)), 11, 0, 0 }, /* dontFrag at 695 */
    { (ASN_TAG_CLASS_CONTEXT | (12 << 2)), 12, 0, 0 }, /* innerHeaderValid at 696 */
    { (ASN_TAG_CLASS_CONTEXT | (13 << 2)), 13, 0, 0 }, /* tunnel6to4 at 697 */
    { (ASN_TAG_CLASS_CONTEXT | (14 << 2)), 14, 0, 0 }, /* tunnelSa6to4 at 698 */
    { (ASN_TAG_CLASS_CONTEXT | (15 << 2)), 15, 0, 0 }, /* greKeyUdpDestPort at 699 */
    { (ASN_TAG_CLASS_CONTEXT | (16 << 2)), 16, 0, 0 }, /* greProtoUdpSrcPort at 700 */
    { (ASN_TAG_CLASS_CONTEXT | (17 << 2)), 17, 0, 0 }, /* ipsa at 701 */
    { (ASN_TAG_CLASS_CONTEXT | (18 << 2)), 18, 0, 0 } /* ipda at 703 */
};
static asn_SEQUENCE_specifics_t asn_SPC_DSL3EditTunnelV4_specs_1 = {
	sizeof(struct DSL3EditTunnelV4),
	offsetof(struct DSL3EditTunnelV4, _asn_ctx),
	asn_MAP_DSL3EditTunnelV4_tag2el_1,
	19,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_DSL3EditTunnelV4 = {
	"DSL3EditTunnelV4",
	"DSL3EditTunnelV4",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_DSL3EditTunnelV4_tags_1,
	sizeof(asn_DEF_DSL3EditTunnelV4_tags_1)
		/sizeof(asn_DEF_DSL3EditTunnelV4_tags_1[0]), /* 1 */
	asn_DEF_DSL3EditTunnelV4_tags_1,	/* Same as above */
	sizeof(asn_DEF_DSL3EditTunnelV4_tags_1)
		/sizeof(asn_DEF_DSL3EditTunnelV4_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_DSL3EditTunnelV4_1,
	19,	/* Elements count */
	&asn_SPC_DSL3EditTunnelV4_specs_1	/* Additional specs */
};

