/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-STATS"
 * 	found in "../common/hal_msg_stats.asn1"
 */

#ifndef	_HalMsgStatsNhKey_H_
#define	_HalMsgStatsNhKey_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgStatsNhKey */
typedef struct HalMsgStatsNhKey {
	long	 nh;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgStatsNhKey_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgStatsNhKey;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgStatsNhKey_H_ */
