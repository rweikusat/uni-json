/*
  parser bindings

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed.
*/
#ifndef uni_json_binding_h
#define uni_json_binding_h

/*  includes */
#include <stdint.h>

/*  types */
/**  parser */
struct uni_json_p_binding
{
    /*  objects */
    void *(*make_object)(void);
    int (*add_2_object)(void *key, void *value, void *obj);

    /*  arrays */
    void *(*make_array)(void);
    int (*add_2_array)(void *value, void *ary);

    /*  strings */
    void *(*make_string)(void);
    int (*add_2_string)(uint8_t *data, size_t len, void *str);

    /*  simple types */
    void *(*make_null)(void);
    void *(*make_bool)(int true_false);
    void *(*make_number)(uint8_t *data, size_t len);
};

#endif
