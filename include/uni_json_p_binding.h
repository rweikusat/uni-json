/*
  parser bindings

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_binding_h
#define uni_json_binding_h

/*  includes */
#include <inttypes.h>
#include <stddef.h>

/**  constants */
enum {
    UJ_NF_NEG = 1,              /* number is negative */
    UJ_NF_INT = 2               /* number is an integer */
};

/*  types */
/**  parser bindings */
struct uni_json_p_binding
{
    /*  error handler */
    void (*on_error)(unsigned code, size_t pos);

    /*  objects */
    void *(*make_object)(void);
    void (*free_object)(void *);
    int (*add_2_object)(void *key, void *value, void *obj);

    /*  arrays */
    void *(*make_array)(void);
    void (*free_array)(void *);
    int (*add_2_array)(void *value, void *ary);

    /*  strings */
    void *(*make_string)(void);
    void (*free_string)(void *);
    int (*add_2_string)(uint8_t *data, size_t len, void *str);
    int (*add_uni_2_string)(uint32_t uni_char, void *str);

    /*  simple types */
    void *(*make_null)(void);
    void (*free_null)(void *);

    void *(*make_bool)(int true_false);
    void (*free_bool)(void *);

    void *(*make_number)(uint8_t *data, size_t len, unsigned flags);
    void (*free_number)(void *);
};

#endif
