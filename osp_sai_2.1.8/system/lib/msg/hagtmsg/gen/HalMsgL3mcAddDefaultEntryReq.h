/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L3MC"
 * 	found in "../l3/hal_msg_l3mc.asn1"
 */

#ifndef	_HalMsgL3mcAddDefaultEntryReq_H_
#define	_HalMsgL3mcAddDefaultEntryReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgL3mcAddDefaultEntryReq */
typedef struct HalMsgL3mcAddDefaultEntryReq {
	long	 isIpv4En;
	long	 isIpv6En;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgL3mcAddDefaultEntryReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgL3mcAddDefaultEntryReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgL3mcAddDefaultEntryReq_H_ */
