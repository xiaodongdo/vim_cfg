/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L2-VLAN"
 * 	found in "../l2/hal_msg_vlan.asn1"
 */

#ifndef	_HalMsgL2VlanAddPortFailListReq_H_
#define	_HalMsgL2VlanAddPortFailListReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct HalMsgL2VlanAddPortFailReq;

/* HalMsgL2VlanAddPortFailListReq */
typedef struct HalMsgL2VlanAddPortFailListReq {
	long	 session;
	struct vlanAddPortFailList {
		A_SEQUENCE_OF(struct HalMsgL2VlanAddPortFailReq) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} vlanAddPortFailList;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgL2VlanAddPortFailListReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgL2VlanAddPortFailListReq;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <HalMsgL2VlanAddPortFailReq.h>

#endif	/* _HalMsgL2VlanAddPortFailListReq_H_ */
