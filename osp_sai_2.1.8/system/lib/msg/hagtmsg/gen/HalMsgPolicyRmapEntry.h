/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-PBR"
 * 	found in "../l3/hal_msg_pbr.asn1"
 */

#ifndef	_HalMsgPolicyRmapEntry_H_
#define	_HalMsgPolicyRmapEntry_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <HalMsgFlowRule.h>
#include <HalMsgPBRAction.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgPolicyRmapEntry */
typedef struct HalMsgPolicyRmapEntry {
	long	 prevRuleId;
	long	 ruleId;
	HalMsgFlowRule_t	 rule;
	HalMsgPBRAction_t	 action;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgPolicyRmapEntry_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgPolicyRmapEntry;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgPolicyRmapEntry_H_ */
