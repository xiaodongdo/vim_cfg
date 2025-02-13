/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM"
 * 	found in "../lcm.asn1"
 */

#include <asn_internal.h>

#include "LcAttachMsg.h"

static asn_TYPE_member_t asn_MBR_sdkTable_28[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_SDKTableType,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_sdkTable_tags_28[] = {
	(ASN_TAG_CLASS_CONTEXT | (26 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_sdkTable_specs_28 = {
	sizeof(struct sdkTable),
	offsetof(struct sdkTable, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_sdkTable_28 = {
	"sdkTable",
	"sdkTable",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_sdkTable_tags_28,
	sizeof(asn_DEF_sdkTable_tags_28)
		/sizeof(asn_DEF_sdkTable_tags_28[0]) - 1, /* 1 */
	asn_DEF_sdkTable_tags_28,	/* Same as above */
	sizeof(asn_DEF_sdkTable_tags_28)
		/sizeof(asn_DEF_sdkTable_tags_28[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_sdkTable_28,
	1,	/* Single element */
	&asn_SPC_sdkTable_specs_28	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_LcAttachMsg_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, logicSlot),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SlotNo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"logicSlot"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, phySlot),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SlotNo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"phySlot"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, lcType),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BoardType,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"lcType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, stmMode),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"stmMode"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, stmExtFlag),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"stmExtFlag"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, portNum),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"portNum"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, poePortNum),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"poePortNum"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, tmprNum),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tmprNum"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, serialNo),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"serialNo"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, bootromVer),
		(ASN_TAG_CLASS_CONTEXT | (9 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"bootromVer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, hwVer),
		(ASN_TAG_CLASS_CONTEXT | (10 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"hwVer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, epldVer),
		(ASN_TAG_CLASS_CONTEXT | (11 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"epldVer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, epldDate),
		(ASN_TAG_CLASS_CONTEXT | (12 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"epldDate"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, epldTime),
		(ASN_TAG_CLASS_CONTEXT | (13 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"epldTime"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, fpgaVer),
		(ASN_TAG_CLASS_CONTEXT | (14 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"fpgaVer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, swVer),
		(ASN_TAG_CLASS_CONTEXT | (15 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"swVer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, tcamType),
		(ASN_TAG_CLASS_CONTEXT | (16 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tcamType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, sramType),
		(ASN_TAG_CLASS_CONTEXT | (17 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"sramType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, flashSize),
		(ASN_TAG_CLASS_CONTEXT | (18 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"flashSize"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, dramSize),
		(ASN_TAG_CLASS_CONTEXT | (19 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dramSize"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, extCardType),
		(ASN_TAG_CLASS_CONTEXT | (20 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extCardType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, extCardVer),
		(ASN_TAG_CLASS_CONTEXT | (21 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extCardVer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, datapathMode),
		(ASN_TAG_CLASS_CONTEXT | (22 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"datapathMode"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, tmprLimit),
		(ASN_TAG_CLASS_CONTEXT | (23 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_TmprLimit,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tmprLimit"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, cpuType),
		(ASN_TAG_CLASS_CONTEXT | (24 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"cpuType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, platformType),
		(ASN_TAG_CLASS_CONTEXT | (25 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"platformType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, sdkTable),
		(ASN_TAG_CLASS_CONTEXT | (26 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_sdkTable_28,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"sdkTable"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct LcAttachMsg, sdkGlbeInfo),
		(ASN_TAG_CLASS_CONTEXT | (27 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SDKGlbInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"sdkGlbeInfo"
		},
};
static ber_tlv_tag_t asn_DEF_LcAttachMsg_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_LcAttachMsg_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* logicSlot at 382 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* phySlot at 383 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* lcType at 384 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* stmMode at 385 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* stmExtFlag at 386 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* portNum at 387 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* poePortNum at 388 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* tmprNum at 389 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 }, /* serialNo at 390 */
    { (ASN_TAG_CLASS_CONTEXT | (9 << 2)), 9, 0, 0 }, /* bootromVer at 391 */
    { (ASN_TAG_CLASS_CONTEXT | (10 << 2)), 10, 0, 0 }, /* hwVer at 392 */
    { (ASN_TAG_CLASS_CONTEXT | (11 << 2)), 11, 0, 0 }, /* epldVer at 393 */
    { (ASN_TAG_CLASS_CONTEXT | (12 << 2)), 12, 0, 0 }, /* epldDate at 394 */
    { (ASN_TAG_CLASS_CONTEXT | (13 << 2)), 13, 0, 0 }, /* epldTime at 395 */
    { (ASN_TAG_CLASS_CONTEXT | (14 << 2)), 14, 0, 0 }, /* fpgaVer at 396 */
    { (ASN_TAG_CLASS_CONTEXT | (15 << 2)), 15, 0, 0 }, /* swVer at 397 */
    { (ASN_TAG_CLASS_CONTEXT | (16 << 2)), 16, 0, 0 }, /* tcamType at 398 */
    { (ASN_TAG_CLASS_CONTEXT | (17 << 2)), 17, 0, 0 }, /* sramType at 399 */
    { (ASN_TAG_CLASS_CONTEXT | (18 << 2)), 18, 0, 0 }, /* flashSize at 400 */
    { (ASN_TAG_CLASS_CONTEXT | (19 << 2)), 19, 0, 0 }, /* dramSize at 401 */
    { (ASN_TAG_CLASS_CONTEXT | (20 << 2)), 20, 0, 0 }, /* extCardType at 402 */
    { (ASN_TAG_CLASS_CONTEXT | (21 << 2)), 21, 0, 0 }, /* extCardVer at 403 */
    { (ASN_TAG_CLASS_CONTEXT | (22 << 2)), 22, 0, 0 }, /* datapathMode at 404 */
    { (ASN_TAG_CLASS_CONTEXT | (23 << 2)), 23, 0, 0 }, /* tmprLimit at 405 */
    { (ASN_TAG_CLASS_CONTEXT | (24 << 2)), 24, 0, 0 }, /* cpuType at 406 */
    { (ASN_TAG_CLASS_CONTEXT | (25 << 2)), 25, 0, 0 }, /* platformType at 407 */
    { (ASN_TAG_CLASS_CONTEXT | (26 << 2)), 26, 0, 0 }, /* sdkTable at 408 */
    { (ASN_TAG_CLASS_CONTEXT | (27 << 2)), 27, 0, 0 } /* sdkGlbeInfo at 410 */
};
static asn_SEQUENCE_specifics_t asn_SPC_LcAttachMsg_specs_1 = {
	sizeof(struct LcAttachMsg),
	offsetof(struct LcAttachMsg, _asn_ctx),
	asn_MAP_LcAttachMsg_tag2el_1,
	28,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_LcAttachMsg = {
	"LcAttachMsg",
	"LcAttachMsg",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_LcAttachMsg_tags_1,
	sizeof(asn_DEF_LcAttachMsg_tags_1)
		/sizeof(asn_DEF_LcAttachMsg_tags_1[0]), /* 1 */
	asn_DEF_LcAttachMsg_tags_1,	/* Same as above */
	sizeof(asn_DEF_LcAttachMsg_tags_1)
		/sizeof(asn_DEF_LcAttachMsg_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_LcAttachMsg_1,
	28,	/* Elements count */
	&asn_SPC_LcAttachMsg_specs_1	/* Additional specs */
};

