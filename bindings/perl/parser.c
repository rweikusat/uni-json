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

/*  variables */
static struct uni_json_p_binding binding = {
    .on_error =		on_error,

    .make_object =	make_hv,

    .make_bool =	make_bool,
    .make_null =	make_null
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

void *parse(uint8_t *data, size_t len)
{
    return uni_json_parse(data, len, &binding);
}
