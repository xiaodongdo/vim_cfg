/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-ACL"
 * 	found in "../aclqos/hal_msg_acl.asn1"
 */

#ifndef	_HalMsgAclUsePacketVlanReq_H_
#define	_HalMsgAclUsePacketVlanReq_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HalMsgAclUsePacketVlanReq */
typedef struct HalMsgAclUsePacketVlanReq {
	long	 enable;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgAclUsePacketVlanReq_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgAclUsePacketVlanReq;

#ifdef __cplusplus
}
#endif

#endif	/* _HalMsgAclUsePacketVlanReq_H_ */
