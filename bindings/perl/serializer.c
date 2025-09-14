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
static int type_of(void *p);
static int get_bool_value(void *boolean);

/*  variables */
static struct uni_json_s_binding binds = {
    .output =		output,
    .type_of =		type_of,
    .get_bool_value =	get_bool_value
};

/*  routines */
static void output(uint8_t *data, size_t len, void *sink)
{
    dTHX;
    sv_catpvn_nomg(sink, data, len);
}

static int type_of(void *p)
{
    dTHX;
    SV *sv;
    char const *rtype;

    sv = p;
    if (SvROK(sv)) {
        rtype = sv_reftype(sv, 0);

        if (strcmp(rtype, "ARRAY") == 0) return UJ_T_ARY;
        if (strcmp(rtype, "HASH") == 0) return UJ_T_OBJ;
        return UJ_T_UNK;
    }

    if (!SvOK(sv)) return UJ_T_NULL;
    if (SvIsBOOL(sv)) return UJ_T_BOOL;

    if (SvUOK(sv) || SvIOK(sv)) return UJ_T_NUM;
    if (SvNOK(sv))
        return isinfnan(SvNV(sv)) ? UJ_T_UNK : UJ_T_NUM;

    if (SvPOK(sv)) return UJ_T_STR;

    return UJ_T_UNK;
}

static int get_bool_value(void *boolean)
{
    dTHX;
    return SvIV((SV *)boolean) != 0;
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
