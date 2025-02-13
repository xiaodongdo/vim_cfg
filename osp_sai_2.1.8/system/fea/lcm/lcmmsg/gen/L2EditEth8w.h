/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_L2EditEth8w_H_
#define	_L2EditEth8w_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* L2EditEth8w */
typedef struct L2EditEth8w {
	long	 outputVidValid;
	long	 deviceMcastMac;
	long	 overWriteEtherType;
	long	 type;
	long	 macsaValid;
	long	 encryptionEn;
	long	 encryptionKeyIndex;
	long	 macsah;
	long	 pktType;
	long	 outputVid;
	long	 macdah;
	long	 macdal;
	long	 macsal;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} L2EditEth8w_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_L2EditEth8w;

#ifdef __cplusplus
}
#endif

#endif	/* _L2EditEth8w_H_ */
