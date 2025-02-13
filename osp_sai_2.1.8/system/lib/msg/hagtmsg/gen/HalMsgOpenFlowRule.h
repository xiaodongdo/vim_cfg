/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-FLOW"
 * 	found in "../openflow/hal_msg_openflow_flow.asn1"
 */

#ifndef	_HalMsgOpenFlowRule_H_
#define	_HalMsgOpenFlowRule_H_


#include <asn_application.h>

/* Including external dependencies */
#include <HalMsgOpenFlowMacRule.h>
#include <HalMsgOpenFlowTnlRule.h>
#include <HalMsgOpenFlowUdfRule.h>
#include <HalMsgOpenFlowArpRule.h>
#include <HalMsgOpenFlowIpv4Rule.h>
#include <HalMsgOpenFlowIpv6Rule.h>
#include <HalMsgOpenFlowMplsRule.h>
#include <constr_CHOICE.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum unionRule_PR {
	unionRule_PR_NOTHING,	/* No components present */
	unionRule_PR_arpRule,
	unionRule_PR_ipv4Rule,
	unionRule_PR_ipv6Rule,
	unionRule_PR_mplsRule,
	unionRule_PR_mplsMcastRule,
} unionRule_PR;

/* HalMsgOpenFlowRule */
typedef struct HalMsgOpenFlowRule {
	HalMsgOpenFlowMacRule_t	 macRule;
	HalMsgOpenFlowTnlRule_t	 tnlRule;
	HalMsgOpenFlowUdfRule_t	 udfRule;
	struct unionRule {
		unionRule_PR present;
		union {
			HalMsgOpenFlowArpRule_t	 arpRule;
			HalMsgOpenFlowIpv4Rule_t	 ipv4Rule;
			HalMsgOpenFlowIpv6Rule_t	 ipv6Rule;
			HalMsgOpenFlowMplsRule_t	 mplsRule;
			HalMsgOpenFlowMplsRule_t	 mplsMcastRule;
		};
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} unionRule;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgOpenFlowRule_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowRule;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgOpenFlowRule_H_ */
