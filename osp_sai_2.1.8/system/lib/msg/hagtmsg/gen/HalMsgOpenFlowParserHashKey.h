/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-OPENFLOW-PARSER"
 * 	found in "../openflow/hal_msg_openflow_parser.asn1"
 */

#ifndef	_HalMsgOpenFlowParserHashKey_H_
#define	_HalMsgOpenFlowParserHashKey_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgOpenFlowParserHashKey */
typedef struct HalMsgOpenFlowParserHashKey {
	long	 hashKey;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgOpenFlowParserHashKey_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgOpenFlowParserHashKey;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgOpenFlowParserHashKey_H_ */
