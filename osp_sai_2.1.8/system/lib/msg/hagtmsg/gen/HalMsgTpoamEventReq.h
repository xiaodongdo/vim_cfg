/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-TPOAM"
 * 	found in "../mpls/hal_msg_tpoam.asn1"
 */

#ifndef	_HalMsgTpoamEventReq_H_
#define	_HalMsgTpoamEventReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <OCTET_STRING.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgTpoamEventReq */
typedef struct HalMsgTpoamEventReq {
	long	 count;
	OCTET_STRING_t	 rawData;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgTpoamEventReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgTpoamEventReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgTpoamEventReq_H_ */
