/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L2-BRG"
 * 	found in "../l2/hal_msg_bridge.asn1"
 */

#ifndef	_HalMsgBrgCpuPacketReq_H_
#define	_HalMsgBrgCpuPacketReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <HalMsgDefL2MacAddr.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgBrgCpuPacketReq */
typedef struct HalMsgBrgCpuPacketReq {
	long	 index;
	long	 byPassAll;
	long	 ethType;
	long	 ethTypeMask;
	HalMsgDefL2MacAddr_t	 mac;
	HalMsgDefL2MacAddr_t	 macMask;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgBrgCpuPacketReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgBrgCpuPacketReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgBrgCpuPacketReq_H_ */
