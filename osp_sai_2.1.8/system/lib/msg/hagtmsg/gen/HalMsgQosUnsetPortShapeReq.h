/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#ifndef	_HalMsgQosUnsetPortShapeReq_H_
#define	_HalMsgQosUnsetPortShapeReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgQosUnsetPortShapeReq */
typedef struct HalMsgQosUnsetPortShapeReq {
	long	 port;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgQosUnsetPortShapeReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgQosUnsetPortShapeReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgQosUnsetPortShapeReq_H_ */
