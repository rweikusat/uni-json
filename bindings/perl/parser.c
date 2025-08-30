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
    .free_string =	free_obj;

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

#if 0
static void *make_number(int neg, uint8_t *int_part, size_t int_len,
                         uint8_t *frac_part, size_t frac_len,
                         int exp_neg, uint8_t *exp_part, size_t exp_len)
{
    dTHX;
    UV uv;
    IV iv;
    int rc;
    size_t need;
    uint8_t *s_nv, *p;

    if (!(frac_len || exp_len)) {
        rc = grok_number(int_part, int_len, &uv);
        if (!(rc & IS_NUMBER_GREATER_THAN_UV_MAX)) {
            if (!neg) return newSVuv(uv);

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

    need = int_len;
    if (neg) ++need;
    if (frac_len) need += frac_len + 1;
    if (exp_len) need += exp_len + exp_neg + 1;
    p = s_nv = alloca(need + 1);

    if (neg) *p++ = '-';
    memcpy(p, int_part, int_len);
    p += int_len;

    if (frac_len) {
        *p++ = '.';
        memcpy(p, frac_part, frac_len);
        p += frac_len;
    }

    if (exp_len) {
        *p++ = 'e';
        if (exp_neg) *p++ = '-';
        memcpy(p, exp_part, exp_len);
        p += exp_len;
    }

    *p = 0;
    return newSVnv(my_atof(s_nv));
}
#endif

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
