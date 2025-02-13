/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-MPLS"
 * 	found in "../mpls/hal_msg_mpls.asn1"
 */

#ifndef	_HalMsgMplsL2vpnUnBindReq_H_
#define	_HalMsgMplsL2vpnUnBindReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgMplsL2vpnUnBindReq */
typedef struct HalMsgMplsL2vpnUnBindReq {
	long	 port;
	long	 vlan;
	long	 mode;
	long	 type;
	long	 fid;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgMplsL2vpnUnBindReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgMplsL2vpnUnBindReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgMplsL2vpnUnBindReq_H_ */
