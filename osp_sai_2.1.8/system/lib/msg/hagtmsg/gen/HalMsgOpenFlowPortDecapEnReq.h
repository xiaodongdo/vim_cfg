/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-TUNNEL"
 * 	found in "../openflow/hal_msg_openflow_tunnel.asn1"
 */

#ifndef	_HalMsgOpenFlowPortDecapEnReq_H_
#define	_HalMsgOpenFlowPortDecapEnReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgOpenFlowPortDecapEnReq */
typedef struct HalMsgOpenFlowPortDecapEnReq {
	long	 gport;
	long	 enable;
	long	 type;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgOpenFlowPortDecapEnReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowPortDecapEnReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgOpenFlowPortDecapEnReq_H_ */
