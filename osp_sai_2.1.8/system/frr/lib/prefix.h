/*
 * Prefix structure.
 * Copyright (C) 1998 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef _ZEBRA_PREFIX_H
#define _ZEBRA_PREFIX_H

#ifdef SUNOS_5
#include <sys/ethernet.h>
#else
#ifdef GNU_LINUX
#include <net/ethernet.h>
#else
#include <netinet/if_ether.h>
#endif
#endif
#include "sockunion.h"

#ifndef ETHER_ADDR_LEN
#ifdef ETHERADDRL
#define ETHER_ADDR_LEN  ETHERADDRL
#else
#define ETHER_ADDR_LEN 6
#endif
#endif

#define ETHER_ADDR_STRLEN (3*ETHER_ADDR_LEN)
/*
 * there isn't a portable ethernet address type. We define our
 * own to simplify internal handling
 */
struct ethaddr {
	u_char octet[ETHER_ADDR_LEN];
} __attribute__((packed));


/* length is the number of valuable bits of prefix structure
* 18 bytes is current length in structure, if address is ipv4
* 30 bytes is in case of ipv6
*/
#define PREFIX_LEN_ROUTE_TYPE_5_IPV4 (18*8)
#define PREFIX_LEN_ROUTE_TYPE_5_IPV6 (30*8)

/* EVPN address (RFC 7432) */
struct evpn_addr {
	u_char route_type;
	u_char flags;
#define IP_ADDR_NONE      0x0
#define IP_ADDR_V4        0x1
#define IP_ADDR_V6        0x2
#define IP_PREFIX_V4      0x4
#define IP_PREFIX_V6      0x8
	struct ethaddr mac;
	uint32_t eth_tag;
	u_char ip_prefix_length;
	union {
		u_char addr;
		struct in_addr v4_addr;
		struct in6_addr v6_addr;
	} ip;
};

/* EVPN prefix structure. */
struct prefix_evpn {
	u_char family;
	u_char prefixlen;
	struct evpn_addr prefix __attribute__((aligned(8)));
};

/*
 * A struct prefix contains an address family, a prefix length, and an
 * address.  This can represent either a 'network prefix' as defined
 * by CIDR, where the 'host bits' of the prefix are 0
 * (e.g. AF_INET:10.0.0.0/8), or an address and netmask
 * (e.g. AF_INET:10.0.0.9/8), such as might be configured on an
 * interface.
 */

/* different OSes use different names */
#if defined(AF_PACKET)
#define AF_ETHERNET AF_PACKET
#else
#if defined(AF_LINK)
#define AF_ETHERNET AF_LINK
#endif
#endif

/* IPv4 and IPv6 unified prefix structure. */
struct prefix {
	u_char family;
	u_char prefixlen;
	union {
		u_char prefix;
		struct in_addr prefix4;
		struct in6_addr prefix6;
		struct {
			struct in_addr id;
			struct in_addr adv_router;
		} lp;
		struct ethaddr prefix_eth; /* AF_ETHERNET */
		u_char val[8];
		uintptr_t ptr;
		struct evpn_addr prefix_evpn;
	} u __attribute__((aligned(8)));
};

/* IPv4 prefix structure. */
struct prefix_ipv4 {
	u_char family;
	u_char prefixlen;
	struct in_addr prefix __attribute__((aligned(8)));
};

/* IPv6 prefix structure. */
struct prefix_ipv6 {
	u_char family;
	u_char prefixlen;
	struct in6_addr prefix __attribute__((aligned(8)));
};

struct prefix_ls {
	u_char family;
	u_char prefixlen;
	struct in_addr id __attribute__((aligned(8)));
	struct in_addr adv_router;
};

/* Prefix for routing distinguisher. */
struct prefix_rd {
	u_char family;
	u_char prefixlen;
	u_char val[8] __attribute__((aligned(8)));
};

/* Prefix for ethernet. */
struct prefix_eth {
	u_char family;
	u_char prefixlen;
	struct ethaddr eth_addr __attribute__((aligned(8))); /* AF_ETHERNET */
};

/* Prefix for a generic pointer */
struct prefix_ptr {
	u_char family;
	u_char prefixlen;
	uintptr_t prefix __attribute__((aligned(8)));
};

struct prefix_sg {
	u_char family;
	u_char prefixlen;
	struct in_addr src __attribute((aligned(8)));
	struct in_addr grp;
};

/* helper to get type safety/avoid casts on calls
 * (w/o this, functions accepting all prefix types need casts on the caller
 * side, which strips type safety since the cast will accept any pointer
 * type.)
 */
union prefixptr {
	struct prefix *p;
	struct prefix_ipv4 *p4;
	struct prefix_ipv6 *p6;
	struct prefix_evpn *evp;
} __attribute__((transparent_union));

union prefixconstptr {
	const struct prefix *p;
	const struct prefix_ipv4 *p4;
	const struct prefix_ipv6 *p6;
	const struct prefix_evpn *evp;
} __attribute__((transparent_union));

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif /* INET_ADDRSTRLEN */

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif /* INET6_ADDRSTRLEN */

#ifndef INET6_BUFSIZ
#define INET6_BUFSIZ 51
#endif /* INET6_BUFSIZ */

/* Maximum prefix string length (IPv6) */
#define PREFIX_STRLEN 51

/* Max bit/byte length of IPv4 address. */
#define IPV4_MAX_BYTELEN    4
#define IPV4_MAX_BITLEN    32
#define IPV4_MAX_PREFIXLEN 32
#define IPV4_ADDR_CMP(D,S)   memcmp ((D), (S), IPV4_MAX_BYTELEN)
#define IPV4_ADDR_SAME(D,S)  (memcmp ((D), (S), IPV4_MAX_BYTELEN) == 0)
#define IPV4_ADDR_COPY(D,S)  memcpy ((D), (S), IPV4_MAX_BYTELEN)

#define IPV4_NET0(a)    ((((u_int32_t) (a)) & 0xff000000) == 0x00000000)
#define IPV4_NET127(a)  ((((u_int32_t) (a)) & 0xff000000) == 0x7f000000)
#define IPV4_LINKLOCAL(a) ((((u_int32_t) (a)) & 0xffff0000) == 0xa9fe0000)
#define IPV4_CLASS_DE(a)  ((((u_int32_t) (a)) & 0xe0000000) == 0xe0000000)

/* Max bit/byte length of IPv6 address. */
#define IPV6_MAX_BYTELEN    16
#define IPV6_MAX_BITLEN    128
#define IPV6_MAX_PREFIXLEN 128
#define IPV6_ADDR_CMP(D,S)   memcmp ((D), (S), IPV6_MAX_BYTELEN)
#define IPV6_ADDR_SAME(D,S)  (memcmp ((D), (S), IPV6_MAX_BYTELEN) == 0)
#define IPV6_ADDR_COPY(D,S)  memcpy ((D), (S), IPV6_MAX_BYTELEN)

/* Count prefix size from mask length */
#define PSIZE(a) (((a) + 7) / (8))

/* Prefix's family member. */
#define PREFIX_FAMILY(p)  ((p)->family)

/* glibc defines s6_addr32 to __in6_u.__u6_addr32 if __USE_{MISC || GNU} */
#ifndef s6_addr32
#if defined(SUNOS_5)
/* Some SunOS define s6_addr32 only to kernel */
#define s6_addr32 _S6_un._S6_u32
#else
#define s6_addr32 __u6_addr.__u6_addr32
#endif /* SUNOS_5 */
#endif /*s6_addr32*/

/* Prototypes. */
extern int str2family(const char *);
extern int afi2family(afi_t);
extern afi_t family2afi(int);
extern const char *safi2str(safi_t safi);
extern const char *afi2str(afi_t afi);

/* Check bit of the prefix. */
extern unsigned int prefix_bit(const u_char *prefix, const u_char prefixlen);
extern unsigned int prefix6_bit(const struct in6_addr *prefix,
				const u_char prefixlen);

extern struct prefix *prefix_new(void);
extern void prefix_free(struct prefix *);
extern const char *prefix_family_str(const struct prefix *);
extern int prefix_blen(const struct prefix *);
extern int str2prefix(const char *, struct prefix *);

#define PREFIX2STR_BUFFER  PREFIX_STRLEN

extern const char *prefix2str(union prefixconstptr, char *, int);
extern int prefix_match(const struct prefix *, const struct prefix *);
extern int prefix_same(const struct prefix *, const struct prefix *);
extern int prefix_cmp(const struct prefix *, const struct prefix *);
extern int prefix_common_bits(const struct prefix *, const struct prefix *);
extern void prefix_copy(struct prefix *dest, const struct prefix *src);
extern void apply_mask(struct prefix *);

extern struct prefix *sockunion2prefix(const union sockunion *dest,
				       const union sockunion *mask);
extern struct prefix *sockunion2hostprefix(const union sockunion *,
					   struct prefix *p);
extern void prefix2sockunion(const struct prefix *, union sockunion *);

extern int str2prefix_eth(const char *, struct prefix_eth *);

extern struct prefix_ipv4 *prefix_ipv4_new(void);
extern void prefix_ipv4_free(struct prefix_ipv4 *);
extern int str2prefix_ipv4(const char *, struct prefix_ipv4 *);
extern void apply_mask_ipv4(struct prefix_ipv4 *);

#define PREFIX_COPY_IPV4(DST, SRC)                                             \
	*((struct prefix_ipv4 *)(DST)) = *((const struct prefix_ipv4 *)(SRC));

extern int prefix_ipv4_any(const struct prefix_ipv4 *);
extern void apply_classful_mask_ipv4(struct prefix_ipv4 *);

extern u_char ip_masklen(struct in_addr);
extern void masklen2ip(const int, struct in_addr *);
/* returns the network portion of the host address */
extern in_addr_t ipv4_network_addr(in_addr_t hostaddr, int masklen);
/* given the address of a host on a network and the network mask length,
 * calculate the broadcast address for that network;
 * special treatment for /31: returns the address of the other host
 * on the network by flipping the host bit */
extern in_addr_t ipv4_broadcast_addr(in_addr_t hostaddr, int masklen);

extern int netmask_str2prefix_str(const char *, const char *, char *);

extern struct prefix_ipv6 *prefix_ipv6_new(void);
extern void prefix_ipv6_free(struct prefix_ipv6 *);
extern int str2prefix_ipv6(const char *, struct prefix_ipv6 *);
extern void apply_mask_ipv6(struct prefix_ipv6 *);

#define PREFIX_COPY_IPV6(DST, SRC)                                             \
	*((struct prefix_ipv6 *)(DST)) = *((const struct prefix_ipv6 *)(SRC));

extern int ip6_masklen(struct in6_addr);
extern void masklen2ip6(const int, struct in6_addr *);

extern const char *inet6_ntoa(struct in6_addr);

extern int prefix_str2mac(const char *str, struct ethaddr *mac);
extern char *prefix_mac2str(const struct ethaddr *mac, char *buf, int size);

static inline int ipv6_martian(struct in6_addr *addr)
{
	struct in6_addr localhost_addr;

	inet_pton(AF_INET6, "::1", &localhost_addr);

	if (IPV6_ADDR_SAME(&localhost_addr, addr))
		return 1;

	return 0;
}

extern int all_digit(const char *);

/* NOTE: This routine expects the address argument in network byte order. */
static inline int ipv4_martian(struct in_addr *addr)
{
	in_addr_t ip = ntohl(addr->s_addr);

	if (IPV4_NET0(ip) || IPV4_NET127(ip) || IPV4_CLASS_DE(ip)) {
		return 1;
	}
	return 0;
}

static inline int is_default_prefix(struct prefix *p)
{
	if (!p)
		return 0;

	if (((p->family == AF_INET) && (p->u.prefix4.s_addr == INADDR_ANY))
	    || ((p->family == AF_INET6)
		&& !memcmp(&p->u.prefix6, &in6addr_any,
			   sizeof(struct in6_addr))))
		return 1;

	return 0;
}

#endif /* _ZEBRA_PREFIX_H */
