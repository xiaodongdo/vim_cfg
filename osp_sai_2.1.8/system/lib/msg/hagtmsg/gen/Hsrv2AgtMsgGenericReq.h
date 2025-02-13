/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HSRV2AGT-MSG"
 * 	found in "../hsrv2AgtMsg.asn1"
 */

#ifndef	_Hsrv2AgtMsgGenericReq_H_
#define	_Hsrv2AgtMsgGenericReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <HalMsgDefErrorCode.h>
#include <PrintableString.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Hsrv2AgtMsgGenericReq */
typedef struct Hsrv2AgtMsgGenericReq {
	HalMsgDefErrorCode_t	 err;
	PrintableString_t	 msg;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Hsrv2AgtMsgGenericReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Hsrv2AgtMsgGenericReq;

#ifdef __cplusplus
}
#endif

#endif	/* _Hsrv2AgtMsgGenericReq_H_ */
