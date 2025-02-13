/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#include <asn_internal.h>

#include "DSSrcPort.h"

static asn_TYPE_member_t asn_MBR_DSSrcPort_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, qosPolicy),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"qosPolicy"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, keepVlanTag),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keepVlanTag"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, ingressFilterEn),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ingressFilterEn"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, macSecurityDiscard),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"macSecurityDiscard"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, l2AclEn),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l2AclEn"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, vlanTagCtl),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"vlanTagCtl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, defaultPcp),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"defaultPcp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, protocolVlanBase),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"protocolVlanBase"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, protocolVlanEn),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"protocolVlanEn"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, l2AclHighPrio),
		(ASN_TAG_CLASS_CONTEXT | (9 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l2AclHighPrio"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, bridgeEn),
		(ASN_TAG_CLASS_CONTEXT | (10 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"bridgeEn"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, defaultVid),
		(ASN_TAG_CLASS_CONTEXT | (11 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"defaultVid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, l2AclLabel),
		(ASN_TAG_CLASS_CONTEXT | (12 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l2AclLabel"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, l2QosLabel),
		(ASN_TAG_CLASS_CONTEXT | (13 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l2QosLabel"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, defaultRepalceTagEn),
		(ASN_TAG_CLASS_CONTEXT | (14 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"defaultRepalceTagEn"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, portCrossConnect),
		(ASN_TAG_CLASS_CONTEXT | (15 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"portCrossConnect"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, routedPort),
		(ASN_TAG_CLASS_CONTEXT | (16 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"routedPort"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, l2SpanEn),
		(ASN_TAG_CLASS_CONTEXT | (17 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l2SpanEn"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, allowMcastMacsa),
		(ASN_TAG_CLASS_CONTEXT | (18 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"allowMcastMacsa"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, sourcePortBundleId),
		(ASN_TAG_CLASS_CONTEXT | (19 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"sourcePortBundleId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, l2SpanId),
		(ASN_TAG_CLASS_CONTEXT | (20 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l2SpanId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, l2QosLookupEn),
		(ASN_TAG_CLASS_CONTEXT | (21 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l2QosLookupEn"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, useOuterTtl),
		(ASN_TAG_CLASS_CONTEXT | (22 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"useOuterTtl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, learningDisable),
		(ASN_TAG_CLASS_CONTEXT | (23 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"learningDisable"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, forceMackey),
		(ASN_TAG_CLASS_CONTEXT | (24 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"forceMackey"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, l2QosHighPriority),
		(ASN_TAG_CLASS_CONTEXT | (25 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l2QosHighPriority"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, srcQueueSelect),
		(ASN_TAG_CLASS_CONTEXT | (26 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"srcQueueSelect"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, vplsPortType),
		(ASN_TAG_CLASS_CONTEXT | (27 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"vplsPortType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, vlanFlowPolicerValid),
		(ASN_TAG_CLASS_CONTEXT | (28 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"vlanFlowPolicerValid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DSSrcPort, portPolicerValid),
		(ASN_TAG_CLASS_CONTEXT | (29 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"portPolicerValid"
		},
};
static ber_tlv_tag_t asn_DEF_DSSrcPort_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_DSSrcPort_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* qosPolicy at 461 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* keepVlanTag at 462 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* ingressFilterEn at 463 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* macSecurityDiscard at 464 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* l2AclEn at 465 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* vlanTagCtl at 466 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* defaultPcp at 467 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* protocolVlanBase at 468 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 }, /* protocolVlanEn at 469 */
    { (ASN_TAG_CLASS_CONTEXT | (9 << 2)), 9, 0, 0 }, /* l2AclHighPrio at 470 */
    { (ASN_TAG_CLASS_CONTEXT | (10 << 2)), 10, 0, 0 }, /* bridgeEn at 471 */
    { (ASN_TAG_CLASS_CONTEXT | (11 << 2)), 11, 0, 0 }, /* defaultVid at 472 */
    { (ASN_TAG_CLASS_CONTEXT | (12 << 2)), 12, 0, 0 }, /* l2AclLabel at 473 */
    { (ASN_TAG_CLASS_CONTEXT | (13 << 2)), 13, 0, 0 }, /* l2QosLabel at 474 */
    { (ASN_TAG_CLASS_CONTEXT | (14 << 2)), 14, 0, 0 }, /* defaultRepalceTagEn at 475 */
    { (ASN_TAG_CLASS_CONTEXT | (15 << 2)), 15, 0, 0 }, /* portCrossConnect at 476 */
    { (ASN_TAG_CLASS_CONTEXT | (16 << 2)), 16, 0, 0 }, /* routedPort at 477 */
    { (ASN_TAG_CLASS_CONTEXT | (17 << 2)), 17, 0, 0 }, /* l2SpanEn at 478 */
    { (ASN_TAG_CLASS_CONTEXT | (18 << 2)), 18, 0, 0 }, /* allowMcastMacsa at 479 */
    { (ASN_TAG_CLASS_CONTEXT | (19 << 2)), 19, 0, 0 }, /* sourcePortBundleId at 480 */
    { (ASN_TAG_CLASS_CONTEXT | (20 << 2)), 20, 0, 0 }, /* l2SpanId at 481 */
    { (ASN_TAG_CLASS_CONTEXT | (21 << 2)), 21, 0, 0 }, /* l2QosLookupEn at 482 */
    { (ASN_TAG_CLASS_CONTEXT | (22 << 2)), 22, 0, 0 }, /* useOuterTtl at 483 */
    { (ASN_TAG_CLASS_CONTEXT | (23 << 2)), 23, 0, 0 }, /* learningDisable at 484 */
    { (ASN_TAG_CLASS_CONTEXT | (24 << 2)), 24, 0, 0 }, /* forceMackey at 485 */
    { (ASN_TAG_CLASS_CONTEXT | (25 << 2)), 25, 0, 0 }, /* l2QosHighPriority at 486 */
    { (ASN_TAG_CLASS_CONTEXT | (26 << 2)), 26, 0, 0 }, /* srcQueueSelect at 487 */
    { (ASN_TAG_CLASS_CONTEXT | (27 << 2)), 27, 0, 0 }, /* vplsPortType at 488 */
    { (ASN_TAG_CLASS_CONTEXT | (28 << 2)), 28, 0, 0 }, /* vlanFlowPolicerValid at 489 */
    { (ASN_TAG_CLASS_CONTEXT | (29 << 2)), 29, 0, 0 } /* portPolicerValid at 491 */
};
static asn_SEQUENCE_specifics_t asn_SPC_DSSrcPort_specs_1 = {
	sizeof(struct DSSrcPort),
	offsetof(struct DSSrcPort, _asn_ctx),
	asn_MAP_DSSrcPort_tag2el_1,
	30,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_DSSrcPort = {
	"DSSrcPort",
	"DSSrcPort",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_DSSrcPort_tags_1,
	sizeof(asn_DEF_DSSrcPort_tags_1)
		/sizeof(asn_DEF_DSSrcPort_tags_1[0]), /* 1 */
	asn_DEF_DSSrcPort_tags_1,	/* Same as above */
	sizeof(asn_DEF_DSSrcPort_tags_1)
		/sizeof(asn_DEF_DSSrcPort_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_DSSrcPort_1,
	30,	/* Elements count */
	&asn_SPC_DSSrcPort_specs_1	/* Additional specs */
};

