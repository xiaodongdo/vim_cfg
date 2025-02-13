/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM"
 * 	found in "../lcm.asn1"
 */

#ifndef	_LcOnlineMsg_H_
#define	_LcOnlineMsg_H_


#include <asn_application.h>

/* Including external dependencies */
#include <TmprLimit.h>
#include <NativeInteger.h>
#include <PrintableString.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* LcOnlineMsg */
typedef struct LcOnlineMsg {
	TmprLimit_t	 tmprLimit;
	long	 epldVer;
	long	 epldDate;
	long	 epldTime;
	long	 epldstatus;
	long	 flashSize;
	long	 dramSize;
	long	 hwVer;
	PrintableString_t	 swVer;
	long	 platformType;
	long	 chipDeviceId;
	long	 chipVersionId;
	PrintableString_t	 chipName;
	PrintableString_t	 bootromVer;
	PrintableString_t	 serialNo;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} LcOnlineMsg_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_LcOnlineMsg;

#ifdef __cplusplus
}
#endif

#endif	/* _LcOnlineMsg_H_ */
