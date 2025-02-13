/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-TYPES"
 * 	found in "../common/hal_msg_types.asn1"
 */

#ifndef	_HalMsgDefErrorCode_H_
#define	_HalMsgDefErrorCode_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum HalMsgDefErrorCode {
	HalMsgDefErrorCode_success	= 0,
	HalMsgDefErrorCode_noMemory	= 1,
	HalMsgDefErrorCode_genericErr	= 2,
	HalMsgDefErrorCode_maxErrNum	= 3
} HalMsgDefErrorCode_e;

/* HalMsgDefErrorCode */
typedef long	 HalMsgDefErrorCode_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgDefErrorCode;
asn_struct_free_f HalMsgDefErrorCode_free;
asn_struct_print_f HalMsgDefErrorCode_print;
asn_constr_check_f HalMsgDefErrorCode_constraint;
ber_type_decoder_f HalMsgDefErrorCode_decode_ber;
der_type_encoder_f HalMsgDefErrorCode_encode_der;
xer_type_decoder_f HalMsgDefErrorCode_decode_xer;
xer_type_encoder_f HalMsgDefErrorCode_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgDefErrorCode_H_ */
