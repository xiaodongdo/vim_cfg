/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_L2MacTcamEntry_H_
#define	_L2MacTcamEntry_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <DSMacKey.h>
#include <DSMacDA.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* L2MacTcamEntry */
typedef struct L2MacTcamEntry {
	long	 index;
	long	 chipid;
	DSMacKey_t	 macKeyData;
	DSMacKey_t	 macKeyMask;
	DSMacDA_t	 macda;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} L2MacTcamEntry_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_L2MacTcamEntry;

#ifdef __cplusplus
}
#endif

#endif	/* _L2MacTcamEntry_H_ */
