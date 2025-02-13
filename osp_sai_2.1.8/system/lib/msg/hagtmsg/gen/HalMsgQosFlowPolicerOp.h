/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#ifndef	_HalMsgQosFlowPolicerOp_H_
#define	_HalMsgQosFlowPolicerOp_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum HalMsgQosFlowPolicerOp {
	HalMsgQosFlowPolicerOp_create	= 0,
	HalMsgQosFlowPolicerOp_remove	= 1,
	HalMsgQosFlowPolicerOp_update	= 2
} HalMsgQosFlowPolicerOp_e;

/* HalMsgQosFlowPolicerOp */
typedef long	 HalMsgQosFlowPolicerOp_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgQosFlowPolicerOp;
asn_struct_free_f HalMsgQosFlowPolicerOp_free;
asn_struct_print_f HalMsgQosFlowPolicerOp_print;
asn_constr_check_f HalMsgQosFlowPolicerOp_constraint;
ber_type_decoder_f HalMsgQosFlowPolicerOp_decode_ber;
der_type_encoder_f HalMsgQosFlowPolicerOp_encode_der;
xer_type_decoder_f HalMsgQosFlowPolicerOp_decode_xer;
xer_type_encoder_f HalMsgQosFlowPolicerOp_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgQosFlowPolicerOp_H_ */
