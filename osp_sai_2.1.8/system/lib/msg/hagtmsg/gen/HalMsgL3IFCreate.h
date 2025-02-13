/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L3IF"
 * 	found in "../intf/hal_msg_l3if.asn1"
 */

#ifndef	_HalMsgL3IFCreate_H_
#define	_HalMsgL3IFCreate_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <HalL3IFInfoDefine.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgL3IFCreate */
typedef struct HalMsgL3IFCreate {
	long	 l3ifid;
	long	 enable;
	HalL3IFInfoDefine_t	 l3ifInfo;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgL3IFCreate_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgL3IFCreate;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgL3IFCreate_H_ */
