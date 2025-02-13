/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM"
 * 	found in "../lcm.asn1"
 */

#ifndef	_ChsmSyncDataType_H_
#define	_ChsmSyncDataType_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum ChsmSyncDataType {
	ChsmSyncDataType_syncStartupConfig	= 0,
	ChsmSyncDataType_syncDhcpsnooping	= 1
} ChsmSyncDataType_e;

/* ChsmSyncDataType */
typedef long	 ChsmSyncDataType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ChsmSyncDataType;
asn_struct_free_f ChsmSyncDataType_free;
asn_struct_print_f ChsmSyncDataType_print;
asn_constr_check_f ChsmSyncDataType_constraint;
ber_type_decoder_f ChsmSyncDataType_decode_ber;
der_type_encoder_f ChsmSyncDataType_encode_der;
xer_type_decoder_f ChsmSyncDataType_decode_xer;
xer_type_encoder_f ChsmSyncDataType_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _ChsmSyncDataType_H_ */
