/*
* The MIT License (MIT)
*
* Copyright (c) Microsoft Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include "unboxedrangeobject.h"
#include "../intrins.h"

/*********************** range Iterator **************************/

/* There are 2 types of iterators, one for C longs, the other for
   Python ints (ie, PyObjects).  This should make iteration fast
   in the normal case, but possible for any numeric value.
*/

static PyObject*
pyjion_rangeiter_next(pyjion_rangeiterobject* r) {
    if (r->index < r->len)
        /* cast to unsigned to avoid possible signed overflow
           in intermediate calculations. */
        return reinterpret_cast<PyObject*>((r->start + (unsigned long) (r->index++) * r->step));
    return (PyObject*) SIG_STOP_ITER;
}

static PyObject*
pyjion_rangeiter_len(pyjion_rangeiterobject* r, PyObject* Py_UNUSED(ignored)) {
    return PyLong_FromLong(r->len - r->index);
}

PyDoc_STRVAR(length_hint_doc,
             "Private method returning an estimate of len(list(it)).");


static PyMethodDef pyjion_rangeiter_methods[] = {
        {"__length_hint__", (PyCFunction) pyjion_rangeiter_len, METH_NOARGS,
         length_hint_doc},
        {nullptr, nullptr} /* sentinel */
};

PyTypeObject PyjionRangeIter_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0) "pyjion_range_iterator", /* tp_name */
        sizeof(pyjion_rangeiterobject),                                 /* tp_basicsize */
        0,                                                              /* tp_itemsize */
        /* methods */
        (destructor) PyObject_Del,            /* tp_dealloc */
        0,                                    /* tp_vectorcall_offset */
        0,                                    /* tp_getattr */
        0,                                    /* tp_setattr */
        0,                                    /* tp_as_async */
        0,                                    /* tp_repr */
        0,                                    /* tp_as_number */
        0,                                    /* tp_as_sequence */
        0,                                    /* tp_as_mapping */
        0,                                    /* tp_hash */
        0,                                    /* tp_call */
        0,                                    /* tp_str */
        PyObject_GenericGetAttr,              /* tp_getattro */
        0,                                    /* tp_setattro */
        0,                                    /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                   /* tp_flags */
        0,                                    /* tp_doc */
        0,                                    /* tp_traverse */
        0,                                    /* tp_clear */
        0,                                    /* tp_richcompare */
        0,                                    /* tp_weaklistoffset */
        PyObject_SelfIter,                    /* tp_iter */
        (iternextfunc) pyjion_rangeiter_next, /* tp_iternext */
        pyjion_rangeiter_methods,             /* tp_methods */
        0,                                    /* tp_members */
};
