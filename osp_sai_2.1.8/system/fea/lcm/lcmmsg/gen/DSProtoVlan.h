/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_DSProtoVlan_H_
#define	_DSProtoVlan_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* DSProtoVlan */
typedef struct DSProtoVlan {
	long	 protoVlanidValid;
	long	 cpuExcpEn;
	long	 discard;
	long	 replaceTagEn;
	long	 ptlvid;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} DSProtoVlan_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_DSProtoVlan;

#ifdef __cplusplus
}
#endif

#endif	/* _DSProtoVlan_H_ */
