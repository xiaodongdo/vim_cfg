/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-QOS"
 * 	found in "../aclqos/hal_msg_qos.asn1"
 */

#ifndef	_HalMsgQosDeinitPortCfgReq_H_
#define	_HalMsgQosDeinitPortCfgReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <HalMsgQosPortShape.h>
#include <HalMsgQosQueueDrop.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgQosDeinitPortCfgReq */
typedef struct HalMsgQosDeinitPortCfgReq {
	long	 port;
	long	 flag;
	long	 domain;
	long	 trust;
	long	 cos;
	long	 wdrrWeight;
	long	 wdrrWeightBmp;
	long	 queueClassBmp;
	long	 queueClassId;
	HalMsgQosPortShape_t	 portShape;
	HalMsgQosQueueDrop_t	 queueDrop;
	long	 queueDropBmp;
	long	 queueShapeBmp;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgQosDeinitPortCfgReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgQosDeinitPortCfgReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgQosDeinitPortCfgReq_H_ */
