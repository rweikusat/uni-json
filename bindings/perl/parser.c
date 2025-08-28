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
static void *make_hv(void);
static void *make_null(void);
static void *make_number(int, uint8_t *, size_t,
                         uint8_t *, size_t,
                         int, uint8_t *, size_t);

static void *make_av(void);
static int add_2_av(void *, void *);

static void free_obj(void *);

/*  variables */
static struct uni_json_p_binding binding = {
    .on_error =		on_error,

    .make_bool =	make_bool,
    .make_null =	make_null,
    .make_number =	make_number,

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

static void *make_number(int neg, uint8_t *int_part, size_t int_len,
                         uint8_t *frac_part, size_t frac_len,
                         int exp_neg, uint8_t *exp_part, size_t exp_len)
{
    dTHX;
    size_t len;
    uint8_t *s, *s_nv;
    UV uv;
    IV iv;
    int rc;

    s = int_part;
    len = int_len;
    if (neg) {
        ++len;
        --s;
    }

    if (frac_len) len += 1 + frac_len;

    if (exp_len) {
        len += exp_len + 1;

        switch (exp_part[-1]) {
        case '-':
        case '+':
            ++len;
        }
    }

    rc = grok_number(s, len, &uv);
    if (!(frac_len || exp_len || (rc & IS_NUMBER_GREATER_THAN_UV_MAX))) {
        if (!neg) return newSVuv(uv);

        if (uv <= IV_MAX) {
            iv = uv;
            return newSViv(-iv);
        }
    }

    s_nv = alloca(len + 1);
    memcpy(s_nv, s, len);
    s_nv[len] = 0;
    return newSVnv(my_atof(s_nv));
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
