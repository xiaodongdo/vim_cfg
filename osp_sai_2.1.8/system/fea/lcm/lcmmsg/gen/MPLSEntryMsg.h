/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_MPLSEntryMsg_H_
#define	_MPLSEntryMsg_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MPLSEntryMsg */
typedef struct MPLSEntryMsg {
	long	 chipid;
	long	 index;
	long	 entryNumSent;
	long	 bitmapUpto;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} MPLSEntryMsg_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_MPLSEntryMsg;

#ifdef __cplusplus
}
#endif

#endif	/* _MPLSEntryMsg_H_ */
