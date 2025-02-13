/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-BOND"
 * 	found in "../openflow/hal_msg_openflow_bond.asn1"
 */

#ifndef	_HalMsgOpenFlowCreateBondReq_H_
#define	_HalMsgOpenFlowCreateBondReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgOpenFlowCreateBondReq */
typedef struct HalMsgOpenFlowCreateBondReq {
	long	 tid;
	long	 gport;
	long	 l3ifid;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgOpenFlowCreateBondReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowCreateBondReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgOpenFlowCreateBondReq_H_ */
