/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-NEXTHOP"
 * 	found in "../openflow/hal_msg_openflow_nexthop.asn1"
 */

#include <asn_internal.h>

#include "HalMsgOpenFlowAddTunnelNhReq.h"

static asn_TYPE_member_t asn_MBR_HalMsgOpenFlowAddTunnelNhReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowAddTunnelNhReq, nhid),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"nhid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowAddTunnelNhReq, offset),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"offset"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowAddTunnelNhReq, statsEn),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"statsEn"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowAddTunnelNhReq, statsId),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"statsId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowAddTunnelNhReq, isUpdate),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"isUpdate"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgOpenFlowAddTunnelNhReq, nhInfo),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_HalMsgOpenFlowTunnelNh,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"nhInfo"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgOpenFlowAddTunnelNhReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgOpenFlowAddTunnelNhReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* nhid at 221 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* offset at 222 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* statsEn at 223 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* statsId at 224 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* isUpdate at 225 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* nhInfo at 227 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgOpenFlowAddTunnelNhReq_specs_1 = {
	sizeof(struct HalMsgOpenFlowAddTunnelNhReq),
	offsetof(struct HalMsgOpenFlowAddTunnelNhReq, _asn_ctx),
	asn_MAP_HalMsgOpenFlowAddTunnelNhReq_tag2el_1,
	6,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowAddTunnelNhReq = {
	"HalMsgOpenFlowAddTunnelNhReq",
	"HalMsgOpenFlowAddTunnelNhReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgOpenFlowAddTunnelNhReq_tags_1,
	sizeof(asn_DEF_HalMsgOpenFlowAddTunnelNhReq_tags_1)
		/sizeof(asn_DEF_HalMsgOpenFlowAddTunnelNhReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgOpenFlowAddTunnelNhReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgOpenFlowAddTunnelNhReq_tags_1)
		/sizeof(asn_DEF_HalMsgOpenFlowAddTunnelNhReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgOpenFlowAddTunnelNhReq_1,
	6,	/* Elements count */
	&asn_SPC_HalMsgOpenFlowAddTunnelNhReq_specs_1	/* Additional specs */
};

