/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-COMM-IF"
 * 	found in "../intf/hal_msg_if.asn1"
 */

#ifndef	_HalMsgIFSetStatsIntval_H_
#define	_HalMsgIFSetStatsIntval_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgIFSetStatsIntval */
typedef struct HalMsgIFSetStatsIntval {
	long	 type;
	long	 gportid;
	long	 intval;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgIFSetStatsIntval_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgIFSetStatsIntval;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgIFSetStatsIntval_H_ */
