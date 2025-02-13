/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-BFD"
 * 	found in "../common/hal_msg_bfd.asn1"
 */

#ifndef	_HalMsgBFDSessionStateReq_H_
#define	_HalMsgBFDSessionStateReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgBFDSessionStateReq */
typedef struct HalMsgBFDSessionStateReq {
	long	 localdisc;
	long	 remotedisc;
	long	 negtx;
	long	 negrx;
	long	 negmulti;
	long	 state;
	long	 diag;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgBFDSessionStateReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgBFDSessionStateReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgBFDSessionStateReq_H_ */
