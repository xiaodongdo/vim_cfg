/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-BFD"
 * 	found in "../common/hal_msg_bfd.asn1"
 */

#ifndef	_HalMsgBFDSessionGet_H_
#define	_HalMsgBFDSessionGet_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgBFDSessionGet */
typedef struct HalMsgBFDSessionGet {
	long	 localdisc;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgBFDSessionGet_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgBFDSessionGet;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgBFDSessionGet_H_ */
