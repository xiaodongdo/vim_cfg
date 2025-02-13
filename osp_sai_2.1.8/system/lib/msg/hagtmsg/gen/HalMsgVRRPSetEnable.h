/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-VRRP"
 * 	found in "../l3/hal_msg_vrrp.asn1"
 */

#ifndef	_HalMsgVRRPSetEnable_H_
#define	_HalMsgVRRPSetEnable_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <HalVRRPVmacDefine.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgVRRPSetEnable */
typedef struct HalMsgVRRPSetEnable {
	long	 l3ifid;
	long	 enable;
	HalVRRPVmacDefine_t	 vmac;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgVRRPSetEnable_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgVRRPSetEnable;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgVRRPSetEnable_H_ */
