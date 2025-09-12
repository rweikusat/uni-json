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

/*  prototypes */
void *parse(uint8_t *, size_t);
SV *serialize(SV *, int);

/*  XS code */
MODULE = JSON::Uni PACKAGE = JSON::Uni

SV *
parse_json(data)
	SV * data
PREINIT:
	uint8_t *d;
        STRLEN len;
CODE:
	d = SvPV(data, len);
        RETVAL = parse(d, len);
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
serialize_json(data, fmt)
	SV * data;
        int fmt
CODE:
	RETVAL = serialize(data, fmt);
OUTPUT:
	RETVAL
