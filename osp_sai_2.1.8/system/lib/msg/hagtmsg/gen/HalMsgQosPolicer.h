/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#ifndef	_HalMsgQosPolicer_H_
#define	_HalMsgQosPolicer_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgQosPolicer */
typedef struct HalMsgQosPolicer {
	long	 cir;
	long	 cbs;
	long	 pir;
	long	 pbs;
	long	 useL3Length;
	long	 isSrTcm;
	long	 isColorBlind;
	long	 dropColor;
	long	 mode;
	long	 isStatsEn;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgQosPolicer_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgQosPolicer;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgQosPolicer_H_ */
