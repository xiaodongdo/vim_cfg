/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-COMM-IF"
 * 	found in "../intf/hal_msg_if.asn1"
 */

#include <asn_internal.h>

#include "HalMsgIFSetFlowCtrlDir.h"

static asn_TYPE_member_t asn_MBR_HalMsgIFSetFlowCtrlDir_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgIFSetFlowCtrlDir, portno),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"portno"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgIFSetFlowCtrlDir, subportno),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"subportno"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgIFSetFlowCtrlDir, slotno),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"slotno"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgIFSetFlowCtrlDir, gport),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"gport"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgIFSetFlowCtrlDir, dir),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dir"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgIFSetFlowCtrlDir, enable),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"enable"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgIFSetFlowCtrlDir_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgIFSetFlowCtrlDir_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* portno at 66 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* subportno at 67 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* slotno at 68 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* gport at 69 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* dir at 70 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* enable at 72 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgIFSetFlowCtrlDir_specs_1 = {
	sizeof(struct HalMsgIFSetFlowCtrlDir),
	offsetof(struct HalMsgIFSetFlowCtrlDir, _asn_ctx),
	asn_MAP_HalMsgIFSetFlowCtrlDir_tag2el_1,
	6,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgIFSetFlowCtrlDir = {
	"HalMsgIFSetFlowCtrlDir",
	"HalMsgIFSetFlowCtrlDir",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgIFSetFlowCtrlDir_tags_1,
	sizeof(asn_DEF_HalMsgIFSetFlowCtrlDir_tags_1)
		/sizeof(asn_DEF_HalMsgIFSetFlowCtrlDir_tags_1[0]), /* 1 */
	asn_DEF_HalMsgIFSetFlowCtrlDir_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgIFSetFlowCtrlDir_tags_1)
		/sizeof(asn_DEF_HalMsgIFSetFlowCtrlDir_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgIFSetFlowCtrlDir_1,
	6,	/* Elements count */
	&asn_SPC_HalMsgIFSetFlowCtrlDir_specs_1	/* Additional specs */
};

