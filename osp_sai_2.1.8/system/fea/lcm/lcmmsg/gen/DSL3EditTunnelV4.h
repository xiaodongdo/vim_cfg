/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "LCM-DEBUG"
 * 	found in "../lcm_debug.asn1"
 */

#ifndef	_DSL3EditTunnelV4_H_
#define	_DSL3EditTunnelV4_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* DSL3EditTunnelV4 */
typedef struct DSL3EditTunnelV4 {
	long	 innerHeaderType;
	long	 greFlagsMipTypeUp;
	long	 greVerMipTypeLower;
	long	 dscp;
	long	 ttl;
	long	 ipProtoType;
	long	 isAtpTunnel;
	long	 ipIdentificType;
	long	 mapDscp;
	long	 mapTtl;
	long	 copyDontFrag;
	long	 dontFrag;
	long	 innerHeaderValid;
	long	 tunnel6to4;
	long	 tunnelSa6to4;
	long	 greKeyUdpDestPort;
	long	 greProtoUdpSrcPort;
	long	 ipsa;
	long	 ipda;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} DSL3EditTunnelV4_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_DSL3EditTunnelV4;

#ifdef __cplusplus
}
#endif

#endif	/* _DSL3EditTunnelV4_H_ */
