/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-DEBUG"
 * 	found in "../common/hal_msg_debug.asn1"
 */

#ifndef	_HalMsgDebugCtrlReq_H_
#define	_HalMsgDebugCtrlReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgDebugCtrlReq */
typedef struct HalMsgDebugCtrlReq {
	long	 lcon;
	long	 linefunon;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgDebugCtrlReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgDebugCtrlReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgDebugCtrlReq_H_ */
