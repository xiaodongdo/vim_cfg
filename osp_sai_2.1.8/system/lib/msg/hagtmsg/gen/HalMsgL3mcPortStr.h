/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L3TYPES"
 * 	found in "../l3/hal_msg_l3_types.asn1"
 */

#ifndef	_HalMsgL3mcPortStr_H_
#define	_HalMsgL3mcPortStr_H_


#include <asn_application.h>

/* Including external dependencies */
#include <PrintableString.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgL3mcPortStr */
typedef PrintableString_t	 HalMsgL3mcPortStr_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgL3mcPortStr;
asn_struct_free_f HalMsgL3mcPortStr_free;
asn_struct_print_f HalMsgL3mcPortStr_print;
asn_constr_check_f HalMsgL3mcPortStr_constraint;
ber_type_decoder_f HalMsgL3mcPortStr_decode_ber;
der_type_encoder_f HalMsgL3mcPortStr_encode_der;
xer_type_decoder_f HalMsgL3mcPortStr_decode_xer;
xer_type_encoder_f HalMsgL3mcPortStr_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgL3mcPortStr_H_ */
