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

#include "uni_json_p_binding.h"
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

static void invoke_error_handler(unsigned code, size_t pos, void *p)
{
    dSP;

    ENTER;
    SAVETEMPS;

    PUSHMARK(SP);
    EXTEND(SP, 2);
    PUSHs(sv_2mortal(newSVuv(code)));
    PUSHs(sv_2mortal(newSVuv(pos)));
    PUTBACK;

    call_sv((SV *)p, G_DISCARD);

    FREETEMPS;
    LEAVE;
}

/*  XS code */
MODULE = JSON::Uni PACKAGE = JSON::Uni

SV *
fmt_consts()
CODE:
	RETVAL = newSVpv((void *)fmt_consts, sizeof(fmt_consts));
OUTPUT:
	RETVAL

SV *
parse_json(data, on_error = &PL_sv_undef)
	SV * data
        SV * on_error
PREINIT:
	struct uni_json_p_binding ours, *binds;
        void *err_p;
	uint8_t *d;
        STRLEN len;
CODE:
	d = SvPV(data, len);

	if (SvOK(on_error)) {
		err_p = on_error;

		ours = default_perl_uj_parser_bindings;
                ours.on_error = invoke_error_handler;
                binds = &ours;
	} else {
		err_p = NULL;
                binds = &default_perl_uj_parser_bindings;
	}

        RETVAL = uni_json_parse(d, len, binds, NULL);
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
