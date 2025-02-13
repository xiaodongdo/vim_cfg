/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_UsrIDMacKey_H_
#define	_UsrIDMacKey_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* UsrIDMacKey */
typedef struct UsrIDMacKey {
	long	 tblid0;
	long	 useridLabel;
	long	 cos;
	long	 cfi;
	long	 l2type;
	long	 macdaUpper;
	long	 macdaLower;
	long	 l3type;
	long	 vlanid;
	long	 macsaUpper;
	long	 macsaLower;
	long	 tableid1;
	long	 delbit;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} UsrIDMacKey_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_UsrIDMacKey;

#ifdef __cplusplus
}
#endif

#endif	/* _UsrIDMacKey_H_ */
