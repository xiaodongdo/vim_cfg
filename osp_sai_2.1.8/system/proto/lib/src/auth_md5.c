

#include "proto.h"
#include "auth_md5.h"

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static const uint8 PADDING[64] = 
{
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/* Encodes input (uint32) into output (uint8). Assumes len is
   a multiple of 4.  */
static void
auth_md5_encode(uint8 *output, uint32 *input, uint32 len)
{
    uint32 i = 0;
    uint32 j = 0;
   
    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[j]   = (uint8)(input[i] & 0xFF);
        output[j+1] = (uint8)((input[i] >> 8) & 0xFF);
        output[j+2] = (uint8)((input[i] >> 16) & 0xFF);
        output[j+3] = (uint8)((input[i] >> 24) & 0xFF);
    }
}

/* Decodes input (uint8) into output (uint32). Assumes len is
   a multiple of 4.  */
static void
auth_md5_decode(uint32 *output, uint8 *input, uint32 len)
{
    uint32 i = 0;
    uint32 j = 0;
   
    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[i] = 
            ((uint32)input[j]) 
          | (((uint32)input[j+1]) << 8)
          | (((uint32)input[j+2]) << 16)
          | (((uint32)input[j+3]) << 24);
    }
}

/* MD5 basic transformation. Transforms state based on block.  */
static void
auth_md5_transform(uint32 state[4], uint8 block[64])
{
    uint32 a = state[0];
    uint32 b = state[1];
    uint32 c = state[2];
    uint32 d = state[3];
    uint32 x[16];

    auth_md5_decode(x, block, 64);

    /* Round 1 */
    FF(a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF(d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF(c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF(b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF(a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF(d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF(c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF(b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF(a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF(d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    GG(a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG(d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG(a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG(d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG(a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG(c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG(b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG(c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH(a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH(d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH(a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH(d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH(c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH(b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH(a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH(b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II(a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II(d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II(b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II(d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II(b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II(a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II(c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II(a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II(b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    sal_memset(x, 0, sizeof(x));
}

/* MD5 initialization. Begins an MD5 operation, writing a new context.  */
void
auth_md5_init(AUTH_MD5_CTX *context)
{
    context->count[0] = context->count[1] = 0;

    /* Load magic initialization constants.
    */
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
   operation, processing another message block, and updating the
   context.  */
void
auth_md5_update(AUTH_MD5_CTX *context,
        const void *inbuf, uint32 inputLen)
{
    uint8 *input = (uint8*)inbuf;
    uint32 i = 0;
    uint32 index = 0;
    uint32 partLen = 0;

    /* Compute number of bytes mod 64 */
    index = (uint32)((context->count[0] >> 3) & 0x3F);

    /* Update number of bits */
    if ((context->count[0] += ((uint32)inputLen << 3)) < ((uint32)inputLen << 3))
    {
        context->count[1]++;
    }

    context->count[1] += ((u_int32_t)inputLen >> 29);

    partLen = 64 - index;

    /* Transform as many times as possible.
    */
    if (inputLen >= partLen)
    {
        sal_memcpy(&context->buffer[index], input, partLen);
        auth_md5_transform(context->state, context->buffer);

        for (i = partLen; i + 63 < inputLen; i += 64)
        {
            auth_md5_transform(context->state, &input[i]);
        }

        index = 0;
    }
    else
    {
        i = 0;
    }

    /* Buffer remaining input */
    sal_memcpy (&context->buffer[index], &input[i], inputLen-i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
   the message digest and zeroizing the context.  */
void
auth_md5_final(uint8 digest[16], AUTH_MD5_CTX *context)
{
   uint8 bits[8];
   uint32 index = 0;
   uint32 padLen = 0;

   /* Save number of bits */
   auth_md5_encode(bits, context->count, 8);

   /* Pad out to 56 mod 64.
    */
   index = (uint32)((context->count[0] >> 3) & 0x3f);
   padLen = (index < 56) ? (56 - index) : (120 - index);
   auth_md5_update(context, PADDING, padLen);

   /* Append length (before padding) */
   auth_md5_update(context, bits, 8);
   
   /* Store state in digest */
   auth_md5_encode(digest, context->state, 16);

   /* Zeroize sensitive information.
    */
   sal_memset(context, 0, sizeof (*context));
}

void
auth_md5_authenticator(uint8 *buf)
{
    AUTH_MD5_CTX c;
    sal_clock_t secs;

    /* Something random.  */
    secs = sal_time_clock();

    auth_md5_init(&c);
    auth_md5_update(&c, (uint8 *)&secs, sizeof (secs));
    auth_md5_final(buf, &c);
}

void
auth_hmac_md5(uint8 *text, uint32 text_len, char *key, uint32 key_len, uint8 *digest)
{
    AUTH_MD5_CTX context;
    uint8 k_ipad[65];    /* inner padding - key XORd with ipad.  */
    uint8 k_opad[65];    /* outer padding - key XORd with opad.  */
    uint8 tk[16];
    uint32 i = 0;

    /* if key is longer than 64 bytes reset it to key=MD5(key).  */
    if (key_len > 64)
    {
      AUTH_MD5_CTX tctx;

      auth_md5_init(&tctx);
      auth_md5_update(&tctx, key, key_len);
      auth_md5_final((uint8*)tk, &tctx);

      key = (char*)tk;
      key_len = 16;
    }

    /* The HMAC_MD5 transform looks like:
     MD5(K XOR opad, MD5(K XOR ipad, text))

     where K is an n byte key ipad is the byte 0x36 repeated 64 times

     opad is the byte 0x5c repeated 64 times
     and text is the data being protected.  */

    /* Start out by storing key in pads.  */
    sal_memset(k_ipad, 0, sizeof(k_ipad));
    sal_memset(k_opad, 0, sizeof(k_opad));
    sal_memcpy(k_ipad, key, key_len);
    sal_memcpy(k_opad, key, key_len);

    /* XOR key with ipad and opad values.  */
    for (i = 0; i < 64; i++)
    {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }
  
    /* Perform inner MD5.  */
    auth_md5_init(&context);                    /* Init context for 1st pass */
    auth_md5_update(&context, k_ipad, 64);      /* Start with inner pad.  */
    auth_md5_update(&context, text, text_len);  /* Then text of datagram.  */
    auth_md5_final(digest, &context);           /* Finish up 1st pass.  */

    /* Perform outer MD5.  */
    auth_md5_init(&context);                /* Init context for 2nd pass */
    auth_md5_update(&context, k_opad, 64);  /* Start with outer pad. */
    auth_md5_update(&context, digest, 16);  /* Then results of 1st hash. */
    auth_md5_final(digest, &context);       /* Finish up 2nd pass.  */
}

