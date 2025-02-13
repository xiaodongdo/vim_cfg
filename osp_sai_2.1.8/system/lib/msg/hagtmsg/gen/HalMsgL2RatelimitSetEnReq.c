/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-RATELIMIT"
 * 	found in "../l2/hal_msg_ratelimit.asn1"
 */

#include <asn_internal.h>

#include "HalMsgL2RatelimitSetEnReq.h"

static asn_TYPE_member_t asn_MBR_HalMsgL2RatelimitSetEnReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2RatelimitSetEnReq, port),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"port"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2RatelimitSetEnReq, type),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"type"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2RatelimitSetEnReq, enable),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"enable"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2RatelimitSetEnReq, op),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"op"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2RatelimitSetEnReq, vlanId),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"vlanId"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgL2RatelimitSetEnReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgL2RatelimitSetEnReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* port at 20 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* type at 21 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* enable at 22 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* op at 23 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 } /* vlanId at 25 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgL2RatelimitSetEnReq_specs_1 = {
	sizeof(struct HalMsgL2RatelimitSetEnReq),
	offsetof(struct HalMsgL2RatelimitSetEnReq, _asn_ctx),
	asn_MAP_HalMsgL2RatelimitSetEnReq_tag2el_1,
	5,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgL2RatelimitSetEnReq = {
	"HalMsgL2RatelimitSetEnReq",
	"HalMsgL2RatelimitSetEnReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgL2RatelimitSetEnReq_tags_1,
	sizeof(asn_DEF_HalMsgL2RatelimitSetEnReq_tags_1)
		/sizeof(asn_DEF_HalMsgL2RatelimitSetEnReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgL2RatelimitSetEnReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgL2RatelimitSetEnReq_tags_1)
		/sizeof(asn_DEF_HalMsgL2RatelimitSetEnReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgL2RatelimitSetEnReq_1,
	5,	/* Elements count */
	&asn_SPC_HalMsgL2RatelimitSetEnReq_specs_1	/* Additional specs */
};

