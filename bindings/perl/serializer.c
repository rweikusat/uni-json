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
    INIT_BUF_SIZE = 128
};

/*  types */
struct aiter {
    SV **p, **e;
};

/*  prototypes */
static void output(uint8_t *data, size_t len, void *sink);
static int type_of(void *p);

static void *start_object_traversal(void *);
static size_t max_kv_pairs(void *);
static int next_kv_pair(void *, struct uj_kv_pair *);

static void *start_array_traversal(void *);
static void *next_value(void *);
static void end_array_traversal(void *);


static void get_num_data(void *num, struct uj_data *data);
static void get_string_data(void *str, struct uj_data *data);
static int get_bool_value(void *boolean);

/*  variables */
struct uni_json_s_binding default_perl_uj_serializer_bindings = {
    .output =			output,
    .type_of =			type_of,
    .alloc =			safemalloc,
    .dealloc =			safefree,

    .start_object_traversal =	start_object_traversal,
    .max_kv_pairs =		max_kv_pairs,
    .next_kv_pair =		next_kv_pair,

    .start_array_traversal =	start_array_traversal,
    .next_value =		next_value,
    .end_array_traversal =	end_array_traversal,

    .get_num_data =		get_num_data,
    .get_string_data =		get_string_data,
    .get_bool_value =		get_bool_value
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

    sv = p;

    if (SvROK(sv)) {
        switch (SvTYPE(SvRV(sv))) {
        case SVt_PVAV:
            return UJ_T_ARY;

        case SVt_PVHV:
            return UJ_T_OBJ;
        }

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

static void *start_object_traversal(void *obj)
{
    dTHX;
    HV *hv;

    hv = (HV *)SvRV((SV *)obj);
    hv_iterinit(hv);
    return hv;
}

static size_t max_kv_pairs(void *obj)
{
    dTHX;
    return HvTOTALKEYS((HV *)SvRV((SV *)obj));
}

static void key_from_he(HE *he, struct uj_data *key)
{
    dTHX;
    STRLEN len, ndx;
    SV *sv;

    key->s = HePV(he, len);

    if (!HeUTF8(he)) {
        ndx = 0;
        while (ndx < len) {
            if (key->s[ndx] > 127) {
                sv = newSVhek(HeKEY_hek(he));
                key->s = SvPVutf8(sv, len);
                key->len = len;

                sv_2mortal(sv);
                return;
            }

            ++ndx;
        }
    }

    key->len = len;
}

static int next_kv_pair(void *oiter, struct uj_kv_pair *kvp)
{
    dTHX;
    HV *hv;
    HE *he;
    STRLEN len;

    hv = oiter;
    he = hv_iternext(hv);
    if (!he) return 0;

    key_from_he(he, &kvp->key);
    kvp->val = HeVAL(he);

    return 1;
}

static void *start_array_traversal(void *ary)
{
    dTHX;
    struct aiter *aiter;
    AV *av;

    av = (AV *)SvRV((SV *)ary);

    aiter = safemalloc(sizeof(*aiter));
    aiter->p = AvARRAY(av);
    aiter->e = aiter->p + av_count(av);

    return aiter;
}

static void *next_value(void *p)
{
    dTHX;
    struct aiter *aiter;
    void *v;

    aiter = p;
    if (aiter->p == aiter->e) return NULL;

    v = *aiter->p++;
    return v;
}

static void end_array_traversal(void *aiter)
{
    dTHX;
    Safefree(aiter);
}

static void get_num_data(void *num, struct uj_data *data)
{
    dTHX;
    char *pv;
    STRLEN len;

    pv = SvPV((SV *)num, len);
    data->s = pv;
    data->len = len;
}

static void get_string_data(void *str, struct uj_data *data)
{
    dTHX;
    char *pv;
    STRLEN len;

    pv = SvPVutf8((SV *)str, len);
    data->s = pv;
    data->len = len;
}

static int get_bool_value(void *boolean)
{
    dTHX;
    return SvIV((SV *)boolean) != 0;
}
