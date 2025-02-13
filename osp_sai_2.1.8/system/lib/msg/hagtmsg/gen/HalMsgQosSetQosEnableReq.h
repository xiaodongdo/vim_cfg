/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#ifndef	_HalMsgQosSetQosEnableReq_H_
#define	_HalMsgQosSetQosEnableReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <BOOLEAN.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgQosSetQosEnableReq */
typedef struct HalMsgQosSetQosEnableReq {
	long	 isVlan;
	long	 gportOrVlanId;
	long	 dir;
	BOOLEAN_t	 enable;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgQosSetQosEnableReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgQosSetQosEnableReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgQosSetQosEnableReq_H_ */
