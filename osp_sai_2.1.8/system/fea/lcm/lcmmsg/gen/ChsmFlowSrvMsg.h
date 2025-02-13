/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM"
 * 	found in "../lcm.asn1"
 */

#ifndef	_ChsmFlowSrvMsg_H_
#define	_ChsmFlowSrvMsg_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ChsmFlowSrvMsg */
typedef struct ChsmFlowSrvMsg {
	long	 dir;
	long	 srv;
	long	 srvPrio;
	long	 aclPrio;
	long	 aclType;
	long	 classidEnable;
	long	 classidPrio;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ChsmFlowSrvMsg_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ChsmFlowSrvMsg;

#ifdef __cplusplus
}
#endif

#endif	/* _ChsmFlowSrvMsg_H_ */
