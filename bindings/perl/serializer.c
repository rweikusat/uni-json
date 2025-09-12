/*
  uni-json serializer bindings for Perl

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"

#include <uni_json_types.h>
#include <uni_json_s_binding.h>
#include <uni_json_serializer.h>

/*  constants */
enum {
    INIT_SIZE = 128
};

/*  variables */
static struct uni_json_s_binding binds;

/*  routines */
SV *serialize(void *val, int fmt)
{
    SV *out;

    out = newSVpv(INIT_SIZE);
    SvPOK_on(out);
    SvUTF8_on(out);

    uni_json_serialize(val, out, &binds, fmt);

    return out;
}
