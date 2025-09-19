/*
  uni-json bindings for Perl -- XS code

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "uni_json_parser.h"
#include "uni_json_serializer.h"

/*  constants */
enum {
    INIT_BUF_SIZE = 128
};

/*  types */
struct a_const {
    char *n;
    uint64_t v;
};

/*  prototypes */
void *parse(uint8_t *, size_t);
SV *serialize(SV *, int);

/*  variables */
extern struct uni_json_p_binding default_perl_uj_parser_bindings;
extern struct uni_json_s_binding default_perl_uj_serializer_bindings;

static struct a_const fmt_consts[] = {
#define n_(x) { .n = #x, .v = x }

    n_(UJ_FMT_FAST),
    n_(UJ_FMT_DET),
    n_(UJ_FMT_PRETTY)

#undef n_
};

/*  XS code */
MODULE = JSON::Uni PACKAGE = JSON::Uni

SV *
fmt_consts()
CODE:
	RETVAL = newSVpv((void *)fmt_consts, sizeof(fmt_consts));
OUTPUT:
	RETVAL

SV *
parse_json(data)
	SV * data
PREINIT:
	uint8_t *d;
        STRLEN len;
CODE:
	d = SvPV(data, len);
        RETVAL = uni_json_parse(d, len, &default_perl_uj_parser_bindings);
OUTPUT:
	RETVAL

unsigned
max_nesting()
CODE:
	RETVAL = uni_json_max_nesting;
OUTPUT:
	RETVAL

void
set_max_nesting(max)
	unsigned max
CODE:
	uni_json_max_nesting = max;

SV *
json_serialize(val, fmt = UJ_FMT_FAST)
	SV * val;
        int fmt;
PREINIT:
        SV * out;
CODE:
	out = newSV(INIT_BUF_SIZE);
	SvPOK_on(out);
	SvUTF8_on(out);

	uni_json_serialize(val, out, &default_perl_uj_serializer_bindings, fmt);

	RETVAL = out;
OUTPUT:
	RETVAL
