/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-FLOW"
 * 	found in "../openflow/hal_msg_openflow_flow.asn1"
 */

#ifndef	_HalMsgOpenFlowAddFlowReq_H_
#define	_HalMsgOpenFlowAddFlowReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <HalMsgOpenFlowEntryId.h>
#include <HalMsgOpenFlowRule.h>
#include <HalMsgOpenFlowAction.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgOpenFlowAddFlowReq */
typedef struct HalMsgOpenFlowAddFlowReq {
	HalMsgOpenFlowEntryId_t	 entryId;
	HalMsgOpenFlowRule_t	 rule;
	HalMsgOpenFlowAction_t	 action;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgOpenFlowAddFlowReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowAddFlowReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgOpenFlowAddFlowReq_H_ */
