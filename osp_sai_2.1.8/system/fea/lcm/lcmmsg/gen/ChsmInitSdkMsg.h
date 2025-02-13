/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM"
 * 	found in "../lcm.asn1"
 */

#ifndef	_ChsmInitSdkMsg_H_
#define	_ChsmInitSdkMsg_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ChsmPropMsg;
struct ChsmFlowSrvMsg;

/* ChsmInitSdkMsg */
typedef struct ChsmInitSdkMsg {
	long	 gchipId;
	long	 supervisorSlot;
	long	 profileMode;
	long	 mep1msNum;
	struct aclPropArray {
		A_SEQUENCE_OF(struct ChsmPropMsg) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} aclPropArray;
	struct flowSrvArray {
		A_SEQUENCE_OF(struct ChsmFlowSrvMsg) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} flowSrvArray;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ChsmInitSdkMsg_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ChsmInitSdkMsg;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <ChsmPropMsg.h>
#include <ChsmFlowSrvMsg.h>

#endif	/* _ChsmInitSdkMsg_H_ */
