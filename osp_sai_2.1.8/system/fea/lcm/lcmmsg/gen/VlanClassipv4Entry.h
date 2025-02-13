/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_VlanClassipv4Entry_H_
#define	_VlanClassipv4Entry_H_


#include <asn_application.h>

/* Including external dependencies */
#include <UsrIDIPv4Key.h>
#include <DSUsrID.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* VlanClassipv4Entry */
typedef struct VlanClassipv4Entry {
	UsrIDIPv4Key_t	 key;
	UsrIDIPv4Key_t	 mask;
	DSUsrID_t	 ds;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} VlanClassipv4Entry_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_VlanClassipv4Entry;

#ifdef __cplusplus
}
#endif

#endif	/* _VlanClassipv4Entry_H_ */
