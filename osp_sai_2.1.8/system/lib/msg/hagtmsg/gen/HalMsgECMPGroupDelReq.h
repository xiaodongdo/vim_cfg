/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-ECMP"
 * 	found in "../l3/hal_msg_ecmp.asn1"
 */

#ifndef	_HalMsgECMPGroupDelReq_H_
#define	_HalMsgECMPGroupDelReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgECMPGroupDelReq */
typedef struct HalMsgECMPGroupDelReq {
	long	 ecmpid;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgECMPGroupDelReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgECMPGroupDelReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgECMPGroupDelReq_H_ */
