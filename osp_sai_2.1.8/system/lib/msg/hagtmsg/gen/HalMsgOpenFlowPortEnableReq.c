/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-PORT"
 * 	found in "../openflow/hal_msg_openflow_port.asn1"
 */

#include <asn_internal.h>

#include "HalMsgOpenFlowPortEnableReq.h"

static asn_TYPE_member_t asn_MBR_HalMsgOpenFlowPortEnableReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowPortEnableReq, gport),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"gport"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowPortEnableReq, label),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"label"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowPortEnableReq, l3ifid),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"l3ifid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowPortEnableReq, enable),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"enable"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowPortEnableReq, portMac),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgDefL2MacAddr,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"portMac"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowPortEnableReq, gchip),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"gchip"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgOpenFlowPortEnableReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgOpenFlowPortEnableReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* gport at 14 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* label at 15 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* l3ifid at 16 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* enable at 17 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* portMac at 18 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* gchip at 20 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgOpenFlowPortEnableReq_specs_1 = {
	sizeof(struct HalMsgOpenFlowPortEnableReq),
	offsetof(struct HalMsgOpenFlowPortEnableReq, _asn_ctx),
	asn_MAP_HalMsgOpenFlowPortEnableReq_tag2el_1,
	6,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowPortEnableReq = {
	"HalMsgOpenFlowPortEnableReq",
	"HalMsgOpenFlowPortEnableReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgOpenFlowPortEnableReq_tags_1,
	sizeof(asn_DEF_HalMsgOpenFlowPortEnableReq_tags_1)
		/sizeof(asn_DEF_HalMsgOpenFlowPortEnableReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgOpenFlowPortEnableReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgOpenFlowPortEnableReq_tags_1)
		/sizeof(asn_DEF_HalMsgOpenFlowPortEnableReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgOpenFlowPortEnableReq_1,
	6,	/* Elements count */
	&asn_SPC_HalMsgOpenFlowPortEnableReq_specs_1	/* Additional specs */
};

