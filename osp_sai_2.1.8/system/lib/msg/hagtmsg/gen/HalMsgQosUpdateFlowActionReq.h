/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#ifndef	_HalMsgQosUpdateFlowActionReq_H_
#define	_HalMsgQosUpdateFlowActionReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <HalMsgQosFlowAction.h>
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct HalMsgQosFlowEntryId;

/* HalMsgQosUpdateFlowActionReq */
typedef struct HalMsgQosUpdateFlowActionReq {
	long	 label;
	struct updateEntryIdList {
		A_SEQUENCE_OF(struct HalMsgQosFlowEntryId) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} updateEntryIdList;
	long	 opAction;
	HalMsgQosFlowAction_t	 action;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgQosUpdateFlowActionReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgQosUpdateFlowActionReq;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <HalMsgQosFlowEntryId.h>

#endif	/* _HalMsgQosUpdateFlowActionReq_H_ */
