/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-TUNNEL"
 * 	found in "../openflow/hal_msg_openflow_tunnel.asn1"
 */

#ifndef	_HalMsgOpenFlowL3ifReq_H_
#define	_HalMsgOpenFlowL3ifReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgOpenFlowL3ifReq */
typedef struct HalMsgOpenFlowL3ifReq {
	long	 l3ifid;
	long	 gport;
	long	 vlanId;
	long	 enable;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgOpenFlowL3ifReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowL3ifReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgOpenFlowL3ifReq_H_ */
