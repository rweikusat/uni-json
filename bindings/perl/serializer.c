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

/*  prototypes */
static void output(uint8_t *data, size_t len, void *sink);

/*  variables */
static struct uni_json_s_binding binds = {
    .output =		output
};

/*  routines */
static void output(uint8_t *data, size_t len, void *sink)
{
    dTHX;
    sv_catpvn_nomg(data, len, sink);
}

SV *serialize(SV *val, int fmt)
{
    dTHX;
    SV *out;

    out = newSV(INIT_SIZE);
    SvPOK_on(out);
    SvUTF8_on(out);

    uni_json_serialize(val, out, &binds, fmt);

    return out;
}
