/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-COMM-IF"
 * 	found in "../intf/hal_msg_if.asn1"
 */

#ifndef	_HalMsgIFPhySync_H_
#define	_HalMsgIFPhySync_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct HalMsgIFPhySyncItem;

/* HalMsgIFPhySync */
typedef struct HalMsgIFPhySync {
	long	 slotno;
	struct phySyncList {
		A_SEQUENCE_OF(struct HalMsgIFPhySyncItem) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} phySyncList;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgIFPhySync_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgIFPhySync;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <HalMsgIFPhySyncItem.h>

#endif	/* _HalMsgIFPhySync_H_ */
