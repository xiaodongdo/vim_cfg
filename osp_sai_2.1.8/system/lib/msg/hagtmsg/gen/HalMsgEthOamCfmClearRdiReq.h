/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-ETH-OAM"
 * 	found in "../l2/hal_msg_eth_oam.asn1"
 */

#ifndef	_HalMsgEthOamCfmClearRdiReq_H_
#define	_HalMsgEthOamCfmClearRdiReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgEthOamCfmClearRdiReq */
typedef struct HalMsgEthOamCfmClearRdiReq {
	long	 glPort;
	long	 vlanid;
	long	 direction;
	long	 level;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgEthOamCfmClearRdiReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgEthOamCfmClearRdiReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgEthOamCfmClearRdiReq_H_ */
