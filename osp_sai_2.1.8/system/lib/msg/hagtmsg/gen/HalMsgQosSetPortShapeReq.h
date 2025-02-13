/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#ifndef	_HalMsgQosSetPortShapeReq_H_
#define	_HalMsgQosSetPortShapeReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <HalMsgQosPortShape.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgQosSetPortShapeReq */
typedef struct HalMsgQosSetPortShapeReq {
	long	 port;
	HalMsgQosPortShape_t	 shape;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgQosSetPortShapeReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgQosSetPortShapeReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgQosSetPortShapeReq_H_ */
