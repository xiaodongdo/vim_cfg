/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-MPLS"
 * 	found in "../mpls/hal_msg_mpls.asn1"
 */

#ifndef	_HalMsgMplsIlmAddReq_H_
#define	_HalMsgMplsIlmAddReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgMplsIlmAddReq */
typedef struct HalMsgMplsIlmAddReq {
	long	 label;
	long	 nexthopid;
	long	 vrf;
	long	 type;
	long	 spaceid;
	long	 model;
	long	 cwen;
	long	 pop;
	long	 fid;
	long	 vplsPort;
	long	 vplsPortType;
	long	 trustExp;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgMplsIlmAddReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgMplsIlmAddReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgMplsIlmAddReq_H_ */
