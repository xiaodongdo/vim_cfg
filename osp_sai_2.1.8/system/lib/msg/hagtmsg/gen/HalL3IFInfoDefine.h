/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L3IF"
 * 	found in "../intf/hal_msg_l3if.asn1"
 */

#ifndef	_HalL3IFInfoDefine_H_
#define	_HalL3IFInfoDefine_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalL3IFInfoDefine */
typedef struct HalL3IFInfoDefine {
	long	 iftype;
	long	 gport;
	long	 vid;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalL3IFInfoDefine_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalL3IFInfoDefine;

#ifdef __cplusplus
}
#endif

#endif	/* _HalL3IFInfoDefine_H_ */
