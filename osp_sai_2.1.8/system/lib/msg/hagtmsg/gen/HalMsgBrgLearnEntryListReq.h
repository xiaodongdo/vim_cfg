/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L2-BRG"
 * 	found in "../l2/hal_msg_bridge.asn1"
 */

#ifndef	_HalMsgBrgLearnEntryListReq_H_
#define	_HalMsgBrgLearnEntryListReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <HalMsgDefErrorCode.h>
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct HalMsgBrgLearnEntryReq;

/* HalMsgBrgLearnEntryListReq */
typedef struct HalMsgBrgLearnEntryListReq {
	HalMsgDefErrorCode_t	 err;
	struct learnArray {
		A_SEQUENCE_OF(struct HalMsgBrgLearnEntryReq) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} learnArray;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgBrgLearnEntryListReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgBrgLearnEntryListReq;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <HalMsgBrgLearnEntryReq.h>

#endif	/* _HalMsgBrgLearnEntryListReq_H_ */
