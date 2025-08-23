/*
  uni-json parser bindings for Perl

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"

#include <uni_json_p_bindings.h>
#include <uni_json_parser.h>

/*  prototypes */
static void *make_hv(void);

/*  variables */
static struct uni_json_p_binding bind = {
    .make_object =	make_hv
};

/*  routines */
static void *make_hv(void)
{
    dTHX;
    return newHV();
}

void *parse(uint8_t *data, size_t len)
{
    return uni_json_parse(data, len, &bind);
}
