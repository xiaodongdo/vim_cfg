/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_SrcVlanCtl_H_
#define	_SrcVlanCtl_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SrcVlanCtl */
typedef struct SrcVlanCtl {
	long	 macBaseStatsEn;
	long	 intfStatsEn;
	long	 stpStateShift;
	long	 vlanStatusTblBase;
	long	 vlanTblBase;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SrcVlanCtl_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SrcVlanCtl;

#ifdef __cplusplus
}
#endif

#endif	/* _SrcVlanCtl_H_ */
