/*
  uni-json parser bindings for Perl

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"

#include <uni_json_p_binding.h>
#include <uni_json_parser.h>

/*  prototypes */
static void on_error(unsigned, size_t);

static void *make_bool(int);
static void *make_null(void);
static void *make_number(uint8_t *, size_t, unsigned);

static void *make_string(void);
static int add_2_string(uint8_t *, size_t, void *);
static int add_uni_2_string(uint32_t, void *);

static void *make_av(void);
static int add_2_av(void *, void *);

static void *make_hv(void);

static void free_obj(void *);

/*  variables */
static struct uni_json_p_binding binding = {
    .on_error =		on_error,

    .make_bool =	make_bool,
    .make_null =	make_null,

    .make_number =	make_number,
    .free_number =	free_obj,

    .make_string =	make_string,
    .free_string =	free_obj,
    .add_2_string =	add_2_string,

    .make_array =	make_av,
    .free_array =	free_obj,
    .add_2_array =	add_2_av
};

/*  routines */
static void on_error(unsigned code, size_t pos)
{
    croak_nocontext("%s (%u) at %zu", uni_json_ec_2_msg(code), code, pos);
}

static void *make_hv(void)
{
    dTHX;
    return newHV();
}

static void *make_null(void)
{
    dTHX;
    return &PL_sv_undef;
}

static void *make_bool(int true_false)
{
    dTHX;
    return true_false ? &PL_sv_yes : &PL_sv_no;
}

static void *make_number(uint8_t *data, size_t len, unsigned flags)
{
    dTHX;
    uint8_t *tmp;
    UV uv;
    IV iv;
    int rc;

    if (flags & UJ_NF_INT) {
        rc = grok_number(data, len, &uv);
        if (!(rc & IS_NUMBER_GREATER_THAN_UV_MAX)) {
            if (!(flags & UJ_NF_NEG)) return newSVuv(uv);

            if (uv < (UV)IV_MAX + 2) {
                if (uv < (UV)IV_MAX + 1)
                    iv = -uv;
                else {
                    iv = -(uv - 1);
                    --iv;
                }

                return newSViv(iv);
            }
        }
    }

    tmp = alloca(len + 1);
    memcpy(tmp, data, len);
    tmp[len] = 0;
    return newSVnv(my_atof(tmp));
}

static void *make_string(void)
{
    dTHX;
    return newSVpvn_utf8("", 0, 1);
}

static int add_2_string(uint8_t *data, size_t len, void *str)
{
    dTHX;
    sv_catpvn(str, data, len);
    return 1;
}

static int add_uni_2_string(uint32_t uni_char, void *str)
{
    dTHX;
    uint8_t buf[UTF8_MAXBYTES + 1], *e;

    e = uvchr_to_utf8(buf, uni_char);
    return add_2_string(buf, e - buf, str);
}

static void *make_av(void)
{
    dTHX;
    return newRV_noinc((SV *)newAV());
}

static int add_2_av(void *v, void *ary)
{
    dTHX;
    av_push((AV *)SvRV((SV*)ary), v);
    return 1;
}

static void free_obj(void *obj)
{
    dTHX;
    SvREFCNT_dec_NN(obj);
}

void *parse(uint8_t *data, size_t len)
{
    return uni_json_parse(data, len, &binding);
}
