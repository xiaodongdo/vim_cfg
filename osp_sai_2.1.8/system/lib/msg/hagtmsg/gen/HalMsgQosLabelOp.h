/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#ifndef	_HalMsgQosLabelOp_H_
#define	_HalMsgQosLabelOp_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum HalMsgQosLabelOp {
	HalMsgQosLabelOp_apply	= 0,
	HalMsgQosLabelOp_unapply	= 1
} HalMsgQosLabelOp_e;

/* HalMsgQosLabelOp */
typedef long	 HalMsgQosLabelOp_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgQosLabelOp;
asn_struct_free_f HalMsgQosLabelOp_free;
asn_struct_print_f HalMsgQosLabelOp_print;
asn_constr_check_f HalMsgQosLabelOp_constraint;
ber_type_decoder_f HalMsgQosLabelOp_decode_ber;
der_type_encoder_f HalMsgQosLabelOp_encode_der;
xer_type_decoder_f HalMsgQosLabelOp_decode_xer;
xer_type_encoder_f HalMsgQosLabelOp_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgQosLabelOp_H_ */
