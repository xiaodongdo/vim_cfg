/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM"
 * 	found in "../lcm.asn1"
 */

#ifndef	_ChsmMgmtCfgMsg_H_
#define	_ChsmMgmtCfgMsg_H_


#include <asn_application.h>

/* Including external dependencies */
#include <PrintableString.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ChsmMgmtCfgMsg */
typedef struct ChsmMgmtCfgMsg {
	PrintableString_t	 cfgs;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ChsmMgmtCfgMsg_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ChsmMgmtCfgMsg;

#ifdef __cplusplus
}
#endif

#endif	/* _ChsmMgmtCfgMsg_H_ */
