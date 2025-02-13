/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-COMM-IF"
 * 	found in "../intf/hal_msg_if.asn1"
 */

#ifndef	_HalMsgCOMMIfSetPortIsolateGroupReq_H_
#define	_HalMsgCOMMIfSetPortIsolateGroupReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgCOMMIfSetPortIsolateGroupReq */
typedef struct HalMsgCOMMIfSetPortIsolateGroupReq {
	struct isolatePortList {
		A_SEQUENCE_OF(long) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} isolatePortList;
	long	 groupID;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgCOMMIfSetPortIsolateGroupReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgCOMMIfSetPortIsolateGroupReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgCOMMIfSetPortIsolateGroupReq_H_ */
