/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L2-BRG"
 * 	found in "../l2/hal_msg_bridge.asn1"
 */

#ifndef	_HalMsgBrgFdbFlushByVlanPortReq_H_
#define	_HalMsgBrgFdbFlushByVlanPortReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgBrgFdbFlushByVlanPortReq */
typedef struct HalMsgBrgFdbFlushByVlanPortReq {
	long	 fid;
	long	 glPort;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgBrgFdbFlushByVlanPortReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgBrgFdbFlushByVlanPortReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgBrgFdbFlushByVlanPortReq_H_ */
