/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-FLOW"
 * 	found in "../openflow/hal_msg_openflow_flow.asn1"
 */

#ifndef	_HalMsgOpenFlowSetE2eReq_H_
#define	_HalMsgOpenFlowSetE2eReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgOpenFlowSetE2eReq */
typedef struct HalMsgOpenFlowSetE2eReq {
	long	 enable;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgOpenFlowSetE2eReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowSetE2eReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgOpenFlowSetE2eReq_H_ */
