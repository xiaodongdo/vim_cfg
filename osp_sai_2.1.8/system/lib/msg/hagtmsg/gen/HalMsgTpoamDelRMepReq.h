/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-TPOAM"
 * 	found in "../mpls/hal_msg_tpoam.asn1"
 */

#ifndef	_HalMsgTpoamDelRMepReq_H_
#define	_HalMsgTpoamDelRMepReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <HalMsgTpoamRMepKey.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgTpoamDelRMepReq */
typedef struct HalMsgTpoamDelRMepReq {
	HalMsgTpoamRMepKey_t	 key;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgTpoamDelRMepReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgTpoamDelRMepReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgTpoamDelRMepReq_H_ */
