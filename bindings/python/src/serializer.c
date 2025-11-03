/*
  uni-json bindings for Python -- serializer

  Copyright (C) 2025 Rainer Weikusat, rweikusat@talktalk.net

  MIT-licensed
*/

/*  includes */
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "compiler.h"
#include "uni_json_s_binding.h"
#include "uni_json_serializer.h"
#include "uni_json_types.h"
#include "work_string.h"

/*  types */
struct key_string {
    struct key_string *p;
    PyObject *s;
};

struct oiter {
    PyObject *dict;
    Py_ssize_t pos;
    struct key_string *k_strs;
};

/*  prototypes */
static int output(uint8_t *, size_t, void *);
static int type_of(void *);

static void *start_object_traversal(void *);
static void end_object_traversal(void *);
static size_t max_kv_pairs(void *);
static int next_kv_pair(void *oiter, struct uj_kv_pair *);

static void get_array_info(void *ary, struct uj_ary_info *);
static void *array_at(void *, size_t);

static int get_num_data(void *, struct uj_num_data *);
static void free_num_data(struct uj_num_data *);

static int get_string_data(void *, struct uj_str_data *);
static int get_bool_value(void *);

/*  variables */
static struct uni_json_s_binding binds = {
    .output =			output,
    .type_of =			type_of,
    .alloc =			malloc,
    .dealloc =			free,

    .start_object_traversal =	start_object_traversal,
    .end_object_traversal =	end_object_traversal,
    .max_kv_pairs =		max_kv_pairs,
    .next_kv_pair =		next_kv_pair,

    .get_array_info =		get_array_info,
    .array_at =			array_at,

    .get_num_data =		get_num_data,
    .free_num_data =		free_num_data,

    .get_string_data =		get_string_data,
    .get_bool_value =		get_bool_value
};

/*  routines */
static int output(uint8_t *data, size_t len, void *sink)
{
    add_2_work_string(data, len, sink);
    return 0;
}

static int type_of(void *obj)
{
    PyTypeObject *tp;
    double d;

    if ((PyObject *)obj == Py_None) return UJ_T_NULL;

    tp = Py_TYPE(obj);
    if (tp == &PyBool_Type) return UJ_T_BOOL;
    if (tp == &PyLong_Type) return UJ_T_NUM;
    if (tp == &PyUnicode_Type) return UJ_T_STR;
    if (tp == &PyList_Type) return UJ_T_ARY;
    if (tp == &PyDict_Type) return UJ_T_OBJ;

    if (tp == &PyFloat_Type) {
        d = PyFloat_AsDouble(obj);
        switch (fpclassify(d)) {
        case FP_NAN:
        case FP_INFINITE:
            return UJ_T_UNK;
        }

        return UJ_T_NUM;
    }

    return UJ_T_UNK;
}

static void *start_object_traversal(void *obj)
{
    struct oiter *oi;

    oi = malloc(sizeof(*oi));
    oi->dict = obj;
    oi->pos = 0;
    oi->k_strs = NULL;

    return oi;
}

static void end_object_traversal(void *oiter)
{
    struct key_string *cur, *next;
    struct oiter *oi;

    oi = oiter;
    next = oi->k_strs;
    while (cur = next, cur) {
        next = next->p;

        Py_DECREF(cur->s);
        free(cur);
    }

    free(oiter);
}

static size_t max_kv_pairs(void *obj)
{
    return PyDict_Size(obj);
}

static int next_kv_pair(void *oiter, struct uj_kv_pair *kvp)
{
    struct oiter *oi;
    struct key_string *k_str;
    PyObject *k, *v;
    Py_ssize_t k_len;
    int rc;

    oi = oiter;
    rc = PyDict_Next(oi->dict, &oi->pos, &k, &v);
    if (!rc) return 0;

    if (type_of(k) != UJ_T_STR) {
        k_str = malloc(sizeof(*k_str));
        k_str->p = oi->k_strs;
        oi->k_strs = k_str;

        k = k_str->s = PyObject_Str(k);
    }

    kvp->key.s = (uint8_t *)PyUnicode_AsUTF8AndSize(k, &k_len);
    kvp->key.len = k_len;
    kvp->val = v;

    return 1;
}

static void get_array_info(void *ary, struct uj_ary_info *ainfo)
{
    ainfo->p = ary;
    ainfo->len = PyList_GET_SIZE(ary);
}

static void *array_at(void *p, size_t ndx)
{
    return PyList_GET_ITEM(p, ndx);
}

static int get_num_data(void *num, struct uj_num_data *ndata)
{
    PyObject *str;
    Py_ssize_t len;
    char const *s;

    str = PyObject_Str(num);
    s = PyUnicode_AsUTF8AndSize(str, &len);

    ndata->rep.s = (uint8_t *)s;
    ndata->rep.len = len;
    ndata->p = str;

    return 0;
}

static void free_num_data(struct uj_num_data *ndata)
{
    Py_DECREF(ndata->p);
}

static int get_string_data(void *str, struct uj_str_data *sdata)
{
    Py_ssize_t len;

    sdata->s = (uint8_t *)PyUnicode_AsUTF8AndSize(str, &len);
    sdata->len = len;

    return 0;
}

static int get_bool_value(void *boolean)
{
    return (PyObject *)boolean == Py_True;
}

PyObject _hidden_ *json_serialize(PyObject *, PyObject *args)
{
    PyObject *obj;
    struct work_string *ws;
    int fmt, rc;

    fmt = UJ_FMT_FAST;
    rc = PyArg_ParseTuple(args, "O|i", &obj, &fmt);
    if (!rc) return NULL;
    switch (fmt) {
    case UJ_FMT_FAST:
    case UJ_FMT_DET:
    case UJ_FMT_PRETTY:
        break;

    default:
        PyErr_SetString(PyExc_ValueError, "wrong value for format argument");
        return NULL;
    }

    ws = make_work_string();
    if (!ws) {
        PyErr_SetString(PyExc_MemoryError, "failed to create work string");
        return NULL;
    }

    uni_json_serialize(obj, ws, &binds, fmt);
    obj = PyUnicode_FromStringAndSize((char *)ws->s, ws->p - ws->s);
    free_work_string(ws);

    return obj;
}
