/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_L2EntryCountReq_H_
#define	_L2EntryCountReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* L2EntryCountReq */
typedef struct L2EntryCountReq {
	long	 chipid;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} L2EntryCountReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_L2EntryCountReq;

#ifdef __cplusplus
}
#endif

#endif	/* _L2EntryCountReq_H_ */
