/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "HAL-MSG-CPU-TRAFFIC"
 * 	found in "../common/hal_msg_cpu_traffic.asn1"
 */

#ifndef	_HalMsgCpuTrafficNhptrToReasonResp_H_
#define	_HalMsgCpuTrafficNhptrToReasonResp_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct HalMsgCpuTrafficNhptrToReason;

/* HalMsgCpuTrafficNhptrToReasonResp */
typedef struct HalMsgCpuTrafficNhptrToReasonResp {
	struct mapList {
		A_SEQUENCE_OF(struct HalMsgCpuTrafficNhptrToReason) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} mapList;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} HalMsgCpuTrafficNhptrToReasonResp_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HalMsgCpuTrafficNhptrToReasonResp;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <HalMsgCpuTrafficNhptrToReason.h>

#endif	/* _HalMsgCpuTrafficNhptrToReasonResp_H_ */
