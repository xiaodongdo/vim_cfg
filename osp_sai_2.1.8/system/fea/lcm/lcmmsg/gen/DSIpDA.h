/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_DSIpDA_H_
#define	_DSIpDA_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* DSIpDA */
typedef struct DSIpDA {
	long	 isatapCheckEn;
	long	 icmpCheckEn;
	long	 mcastRpfFailCpuEn;
	long	 bidiPimGroup;
	long	 bidiPimGroupValid;
	long	 priorityPathEn;
	long	 ipdaExceptionEn;
	long	 equaCostPathNum;
	long	 tunnelPacketType;
	long	 rpfIfid;
	long	 l3IfType;
	long	 tunnelGreOptions;
	long	 tunnelPayLoadOffset;
	long	 ttlCheckEn;
	long	 payLoadSelect;
	long	 dsfwdPtr;
	long	 denyPbr;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} DSIpDA_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_DSIpDA;

#ifdef __cplusplus
}
#endif

#endif	/* _DSIpDA_H_ */
