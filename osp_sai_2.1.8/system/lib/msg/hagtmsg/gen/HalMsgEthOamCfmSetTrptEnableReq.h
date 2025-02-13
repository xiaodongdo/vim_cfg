/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-ETH-OAM"
 * 	found in "../l2/hal_msg_eth_oam.asn1"
 */

#ifndef	_HalMsgEthOamCfmSetTrptEnableReq_H_
#define	_HalMsgEthOamCfmSetTrptEnableReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <HalMsgDefL2MacAddr.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgEthOamCfmSetTrptEnableReq */
typedef struct HalMsgEthOamCfmSetTrptEnableReq {
	long	 glPort;
	long	 vlanid;
	long	 direction;
	long	 level;
	long	 mepid;
	long	 enable;
	long	 packetType;
	long	 transmitMode;
	long	 transmitNum;
	long	 patternType;
	long	 tlvType;
	long	 repeatValue;
	long	 packetSize;
	long	 rate;
	long	 maxRate;
	long	 incrRate;
	long	 txPort;
	long	 timeRange;
	long	 tstType;
	long	 cos;
	HalMsgDefL2MacAddr_t	 routeMac;
	HalMsgDefL2MacAddr_t	 destMac;
	long	 tpid;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgEthOamCfmSetTrptEnableReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgEthOamCfmSetTrptEnableReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgEthOamCfmSetTrptEnableReq_H_ */
