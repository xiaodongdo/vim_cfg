/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-COMM-IF"
 * 	found in "../intf/hal_msg_if.asn1"
 */

#ifndef	_HalMsgIFSetTpidValue_H_
#define	_HalMsgIFSetTpidValue_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgIFSetTpidValue */
typedef struct HalMsgIFSetTpidValue {
	long	 type;
	long	 value;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgIFSetTpidValue_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgIFSetTpidValue;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgIFSetTpidValue_H_ */
