/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-IPT"
 * 	found in "../misc/hal_msg_iptables.asn1"
 */

#ifndef	_HalMsgIptSetMacEqualReq_H_
#define	_HalMsgIptSetMacEqualReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgIptSetMacEqualReq */
typedef struct HalMsgIptSetMacEqualReq {
	long	 enable;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgIptSetMacEqualReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgIptSetMacEqualReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgIptSetMacEqualReq_H_ */
