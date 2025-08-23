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

/*  prototypes */
void *parse(uint8_t *, size_t);

/*  XS code */
MODULE = JSON_Uni

SV *
parse(data)
	SV * data
PREINIT:
	uint8_t *d;
        STRLEN len;
CODE:
	d = SvPV(data, len);
        RETVAL = parse(d, len);
OUTPUT:
	RETVAL
