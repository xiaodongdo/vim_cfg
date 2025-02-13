/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L2-VLAN"
 * 	found in "../l2/hal_msg_vlan.asn1"
 */

#include <asn_internal.h>

#include "HalMsgL2VlanDeleteVidFromPortReq.h"

static asn_TYPE_member_t asn_MBR_HalMsgL2VlanDeleteVidFromPortReq_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2VlanDeleteVidFromPortReq, port),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"port"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2VlanDeleteVidFromPortReq, vid),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"vid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2VlanDeleteVidFromPortReq, setDefEntry),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"setDefEntry"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2VlanDeleteVidFromPortReq, fid),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"fid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2VlanDeleteVidFromPortReq, maxGchipNum),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"maxGchipNum"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2VlanDeleteVidFromPortReq, setVlanStatus),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"setVlanStatus"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct HalMsgL2VlanDeleteVidFromPortReq, localMemberBmp),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"localMemberBmp"
		},
};
static ber_tlv_tag_t asn_DEF_HalMsgL2VlanDeleteVidFromPortReq_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_HalMsgL2VlanDeleteVidFromPortReq_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* port at 83 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* vid at 84 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* setDefEntry at 85 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* fid at 86 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* maxGchipNum at 87 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* setVlanStatus at 88 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 } /* localMemberBmp at 90 */
};
static asn_SEQUENCE_specifics_t asn_SPC_HalMsgL2VlanDeleteVidFromPortReq_specs_1 = {
	sizeof(struct HalMsgL2VlanDeleteVidFromPortReq),
	offsetof(struct HalMsgL2VlanDeleteVidFromPortReq, _asn_ctx),
	asn_MAP_HalMsgL2VlanDeleteVidFromPortReq_tag2el_1,
	7,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_HalMsgL2VlanDeleteVidFromPortReq = {
	"HalMsgL2VlanDeleteVidFromPortReq",
	"HalMsgL2VlanDeleteVidFromPortReq",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0,	/* No PER decoder, -gen-PER to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_HalMsgL2VlanDeleteVidFromPortReq_tags_1,
	sizeof(asn_DEF_HalMsgL2VlanDeleteVidFromPortReq_tags_1)
		/sizeof(asn_DEF_HalMsgL2VlanDeleteVidFromPortReq_tags_1[0]), /* 1 */
	asn_DEF_HalMsgL2VlanDeleteVidFromPortReq_tags_1,	/* Same as above */
	sizeof(asn_DEF_HalMsgL2VlanDeleteVidFromPortReq_tags_1)
		/sizeof(asn_DEF_HalMsgL2VlanDeleteVidFromPortReq_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_HalMsgL2VlanDeleteVidFromPortReq_1,
	7,	/* Elements count */
	&asn_SPC_HalMsgL2VlanDeleteVidFromPortReq_specs_1	/* Additional specs */
};

