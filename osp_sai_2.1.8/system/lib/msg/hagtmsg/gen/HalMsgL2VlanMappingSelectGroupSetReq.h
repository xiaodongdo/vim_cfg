/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-L2-VLAN-MAPPING"
 * 	found in "../l2/hal_msg_vlan_mapping.asn1"
 */

#ifndef	_HalMsgL2VlanMappingSelectGroupSetReq_H_
#define	_HalMsgL2VlanMappingSelectGroupSetReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgL2VlanMappingSelectGroupSetReq */
typedef struct HalMsgL2VlanMappingSelectGroupSetReq {
	long	 groupId;
	long	 add;
	long	 vlanMin0;
	long	 vlanMax0;
	long	 vlanMin1;
	long	 vlanMax1;
	long	 vlanMin2;
	long	 vlanMax2;
	long	 vlanMin3;
	long	 vlanMax3;
	long	 vlanMin4;
	long	 vlanMax4;
	long	 vlanMin5;
	long	 vlanMax5;
	long	 vlanMin6;
	long	 vlanMax6;
	long	 vlanMin7;
	long	 vlanMax7;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgL2VlanMappingSelectGroupSetReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgL2VlanMappingSelectGroupSetReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgL2VlanMappingSelectGroupSetReq_H_ */
