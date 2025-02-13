/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-NEXTHOP"
 * 	found in "../openflow/hal_msg_openflow_nexthop.asn1"
 */

#ifndef	_HalMsgOpenFlowAddPushL2vpnNhReq_H_
#define	_HalMsgOpenFlowAddPushL2vpnNhReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <HalMsgOpenFlowMplsNhLabelParam.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgOpenFlowAddPushL2vpnNhReq */
typedef struct HalMsgOpenFlowAddPushL2vpnNhReq {
	long	 nhid;
	long	 offset;
    long	 mcastNhid;
	long	 mcastOffset;
	long	 tunnelId;
	long	 innerSvlanTagOp;
	long	 innerSvlanId;
	HalMsgOpenFlowMplsNhLabelParam_t	 pwLabel;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgOpenFlowAddPushL2vpnNhReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowAddPushL2vpnNhReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgOpenFlowAddPushL2vpnNhReq_H_ */
