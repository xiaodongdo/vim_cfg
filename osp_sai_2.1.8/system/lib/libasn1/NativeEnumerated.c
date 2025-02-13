/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Please read the NativeInteger.h for the explanation wrt. differences between
 * INTEGER and NativeInteger.
 * Basically, both are decoders and encoders of ASN.1 INTEGER type, but this
 * implementation deals with the standard (machine-specific) representation
 * of them instead of using the platform-independent buffer.
 */
#include <asn_internal.h>
#include <NativeEnumerated.h>

/*
 * NativeEnumerated basic type description.
 */
static ber_tlv_tag_t asn_DEF_NativeEnumerated_tags[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
asn_TYPE_descriptor_t asn_DEF_NativeEnumerated = {
	"ENUMERATED",			/* The ASN.1 type is still ENUMERATED */
	"ENUMERATED",
	NativeInteger_free,
	NativeInteger_print,
	asn_generic_no_constraint,
	NativeInteger_decode_ber,
	NativeInteger_encode_der,
	NativeInteger_decode_xer,
	NativeEnumerated_encode_xer,
	NativeEnumerated_decode_uper,
	0, /* Use generic outmost tag fetcher */
	asn_DEF_NativeEnumerated_tags,
	sizeof(asn_DEF_NativeEnumerated_tags) / sizeof(asn_DEF_NativeEnumerated_tags[0]),
	asn_DEF_NativeEnumerated_tags,	/* Same as above */
	sizeof(asn_DEF_NativeEnumerated_tags) / sizeof(asn_DEF_NativeEnumerated_tags[0]),
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

asn_enc_rval_t
NativeEnumerated_encode_xer(asn_TYPE_descriptor_t *td, void *sptr,
        int ilevel, enum xer_encoder_flags_e flags,
                asn_app_consume_bytes_f *cb, void *app_key) {
	asn_INTEGER_specifics_t *specs=(asn_INTEGER_specifics_t *)td->specifics;
        asn_enc_rval_t er;
        const long *native = (const long *)sptr;
	const asn_INTEGER_enum_map_t *el;

        (void)ilevel;
        (void)flags;

        if(!native) _ASN_ENCODE_FAILED;

	el = INTEGER_map_value2enum(specs, *native);
	if(el) {
		size_t srcsize = el->enum_len + 5;
		char *src = (char *)alloca(srcsize);

		er.encoded = snprintf(src, srcsize, "<%s/>", el->enum_name);
		assert(er.encoded > 0 && (size_t)er.encoded < srcsize);
		if(cb(src, er.encoded, app_key) < 0) _ASN_ENCODE_FAILED;
		_ASN_ENCODED_OK(er);
	} else {
		ASN_DEBUG("ASN.1 forbids dealing with "
			"unknown value of ENUMERATED type");
		_ASN_ENCODE_FAILED;
	}
}

asn_dec_rval_t
NativeEnumerated_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
	asn_per_constraints_t *constraints, void **sptr, asn_per_data_t *pd) {  
	asn_INTEGER_specifics_t *specs = (asn_INTEGER_specifics_t *)td->specifics;
	asn_dec_rval_t rval = { RC_OK, 0 };
	long *native = (long *)*sptr;
	asn_per_constraint_t *ct;
	long value;

	(void)opt_codec_ctx;

	if(constraints) ct = &constraints->value;
	else if(td->per_constraints) ct = &td->per_constraints->value;
	else _ASN_DECODE_FAILED;	/* Mandatory! */
	if(!specs) _ASN_DECODE_FAILED;

	if(!native) {
		native = (long *)(*sptr = CALLOC(1, sizeof(*native)));
		if(!native) _ASN_DECODE_FAILED;
	}

	ASN_DEBUG("Decoding %s as NativeEnumerated", td->name);

	if(ct->flags & APC_EXTENSIBLE) {
		int inext = per_get_few_bits(pd, 1);
		if(inext < 0) _ASN_DECODE_FAILED;
		if(inext) ct = 0;
	}

	if(ct && ct->range_bits >= 0) {
		value = per_get_few_bits(pd, ct->range_bits);
		if(value < 0) _ASN_DECODE_FAILED;
		if(value >= (specs->extension
			? specs->extension - 1 : specs->map_count))
			_ASN_DECODE_FAILED;
	} else {
		if(!specs->extension)
			_ASN_DECODE_FAILED;
		/*
		 * X.691, #10.6: normally small non-negative whole number;
		 */
		value = uper_get_nsnnwn(pd);
		if(value < 0) _ASN_DECODE_FAILED;
		value += specs->extension - 1;
		if(value >= specs->map_count)
			_ASN_DECODE_FAILED;
	}

	*native = specs->value2enum[value].nat_value;
	ASN_DEBUG("Decoded %s = %ld", td->name, *native);

	return rval;
}

