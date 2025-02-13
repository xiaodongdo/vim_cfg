/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#include <asn_internal.h>

#include "DSAclQosIpv6Key.h"

static asn_TYPE_member_t asn_MBR_DSAclQosIpv6Key_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, l2QosLabel),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l2QosLabel"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, l3QosLabel),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l3QosLabel"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, macdaUp),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"macdaUp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, macdaLower),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"macdaLower"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, tableId0),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tableId0"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, tableId1),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tableId1"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, cosCfi),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"cosCfi"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, l3Type),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l3Type"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, l2Type),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l2Type"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, macsaUp),
		(ASN_TAG_CLASS_CONTEXT | (9 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"macsaUp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, macsaLower),
		(ASN_TAG_CLASS_CONTEXT | (10 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"macsaLower"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, mappedVlanid),
		(ASN_TAG_CLASS_CONTEXT | (11 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"mappedVlanid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipv6Flowlabel),
		(ASN_TAG_CLASS_CONTEXT | (12 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipv6Flowlabel"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipv6ExHeader),
		(ASN_TAG_CLASS_CONTEXT | (13 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipv6ExHeader"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, routedPkt),
		(ASN_TAG_CLASS_CONTEXT | (14 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"routedPkt"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, istcp),
		(ASN_TAG_CLASS_CONTEXT | (15 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"istcp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, dscp),
		(ASN_TAG_CLASS_CONTEXT | (16 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dscp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, l4infoMapped),
		(ASN_TAG_CLASS_CONTEXT | (17 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l4infoMapped"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, l4destport),
		(ASN_TAG_CLASS_CONTEXT | (18 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l4destport"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, l4srcport),
		(ASN_TAG_CLASS_CONTEXT | (19 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l4srcport"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, aclLabel),
		(ASN_TAG_CLASS_CONTEXT | (20 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"aclLabel"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, qosLabel),
		(ASN_TAG_CLASS_CONTEXT | (21 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"qosLabel"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipheadErr),
		(ASN_TAG_CLASS_CONTEXT | (22 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipheadErr"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipOption),
		(ASN_TAG_CLASS_CONTEXT | (23 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipOption"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, isudp),
		(ASN_TAG_CLASS_CONTEXT | (24 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"isudp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, isapp),
		(ASN_TAG_CLASS_CONTEXT | (25 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"isapp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, fragInfo),
		(ASN_TAG_CLASS_CONTEXT | (26 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"fragInfo"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipda13),
		(ASN_TAG_CLASS_CONTEXT | (27 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipda13"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipda2),
		(ASN_TAG_CLASS_CONTEXT | (28 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipda2"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipda4),
		(ASN_TAG_CLASS_CONTEXT | (29 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipda4"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipda5),
		(ASN_TAG_CLASS_CONTEXT | (30 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipda5"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipsa13),
		(ASN_TAG_CLASS_CONTEXT | (31 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipsa13"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipsa2),
		(ASN_TAG_CLASS_CONTEXT | (32 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipsa2"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipsa4),
		(ASN_TAG_CLASS_CONTEXT | (33 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipsa4"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSAclQosIpv6Key, ipsa5),
		(ASN_TAG_CLASS_CONTEXT | (34 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ipsa5"
		},
};
static ber_tlv_tag_t asn_DEF_DSAclQosIpv6Key_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_DSAclQosIpv6Key_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* l2QosLabel at 150 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* l3QosLabel at 151 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* macdaUp at 152 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* macdaLower at 153 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* tableId0 at 154 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* tableId1 at 155 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* cosCfi at 156 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* l3Type at 157 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 }, /* l2Type at 158 */
    { (ASN_TAG_CLASS_CONTEXT | (9 << 2)), 9, 0, 0 }, /* macsaUp at 159 */
    { (ASN_TAG_CLASS_CONTEXT | (10 << 2)), 10, 0, 0 }, /* macsaLower at 160 */
    { (ASN_TAG_CLASS_CONTEXT | (11 << 2)), 11, 0, 0 }, /* mappedVlanid at 161 */
    { (ASN_TAG_CLASS_CONTEXT | (12 << 2)), 12, 0, 0 }, /* ipv6Flowlabel at 162 */
    { (ASN_TAG_CLASS_CONTEXT | (13 << 2)), 13, 0, 0 }, /* ipv6ExHeader at 163 */
    { (ASN_TAG_CLASS_CONTEXT | (14 << 2)), 14, 0, 0 }, /* routedPkt at 164 */
    { (ASN_TAG_CLASS_CONTEXT | (15 << 2)), 15, 0, 0 }, /* istcp at 165 */
    { (ASN_TAG_CLASS_CONTEXT | (16 << 2)), 16, 0, 0 }, /* dscp at 166 */
    { (ASN_TAG_CLASS_CONTEXT | (17 << 2)), 17, 0, 0 }, /* l4infoMapped at 167 */
    { (ASN_TAG_CLASS_CONTEXT | (18 << 2)), 18, 0, 0 }, /* l4destport at 168 */
    { (ASN_TAG_CLASS_CONTEXT | (19 << 2)), 19, 0, 0 }, /* l4srcport at 169 */
    { (ASN_TAG_CLASS_CONTEXT | (20 << 2)), 20, 0, 0 }, /* aclLabel at 170 */
    { (ASN_TAG_CLASS_CONTEXT | (21 << 2)), 21, 0, 0 }, /* qosLabel at 171 */
    { (ASN_TAG_CLASS_CONTEXT | (22 << 2)), 22, 0, 0 }, /* ipheadErr at 172 */
    { (ASN_TAG_CLASS_CONTEXT | (23 << 2)), 23, 0, 0 }, /* ipOption at 173 */
    { (ASN_TAG_CLASS_CONTEXT | (24 << 2)), 24, 0, 0 }, /* isudp at 174 */
    { (ASN_TAG_CLASS_CONTEXT | (25 << 2)), 25, 0, 0 }, /* isapp at 175 */
    { (ASN_TAG_CLASS_CONTEXT | (26 << 2)), 26, 0, 0 }, /* fragInfo at 176 */
    { (ASN_TAG_CLASS_CONTEXT | (27 << 2)), 27, 0, 0 }, /* ipda13 at 177 */
    { (ASN_TAG_CLASS_CONTEXT | (28 << 2)), 28, 0, 0 }, /* ipda2 at 178 */
    { (ASN_TAG_CLASS_CONTEXT | (29 << 2)), 29, 0, 0 }, /* ipda4 at 179 */
    { (ASN_TAG_CLASS_CONTEXT | (30 << 2)), 30, 0, 0 }, /* ipda5 at 180 */
    { (ASN_TAG_CLASS_CONTEXT | (31 << 2)), 31, 0, 0 }, /* ipsa13 at 181 */
    { (ASN_TAG_CLASS_CONTEXT | (32 << 2)), 32, 0, 0 }, /* ipsa2 at 182 */
    { (ASN_TAG_CLASS_CONTEXT | (33 << 2)), 33, 0, 0 }, /* ipsa4 at 183 */
    { (ASN_TAG_CLASS_CONTEXT | (34 << 2)), 34, 0, 0 } /* ipsa5 at 185 */
};
static asn_SEQUENCE_specifics_t asn_SPC_DSAclQosIpv6Key_specs_1 = {
	sizeof(struct DSAclQosIpv6Key),
	offsetof(struct DSAclQosIpv6Key, _asn_ctx),
	asn_MAP_DSAclQosIpv6Key_tag2el_1,
	35,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_DSAclQosIpv6Key = {
	"DSAclQosIpv6Key",
	"DSAclQosIpv6Key",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_DSAclQosIpv6Key_tags_1,
	sizeof(asn_DEF_DSAclQosIpv6Key_tags_1)
		/sizeof(asn_DEF_DSAclQosIpv6Key_tags_1[0]), /* 1 */
	asn_DEF_DSAclQosIpv6Key_tags_1,	/* Same as above */
	sizeof(asn_DEF_DSAclQosIpv6Key_tags_1)
		/sizeof(asn_DEF_DSAclQosIpv6Key_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_DSAclQosIpv6Key_1,
	35,	/* Elements count */
	&asn_SPC_DSAclQosIpv6Key_specs_1	/* Additional specs */
};

