/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_TrunkTbl_H_
#define	_TrunkTbl_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TrunkTbl */
typedef struct TrunkTbl {
	long	 destChipid;
	long	 destQueue;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} TrunkTbl_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_TrunkTbl;

#ifdef __cplusplus
}
#endif

#endif	/* _TrunkTbl_H_ */
