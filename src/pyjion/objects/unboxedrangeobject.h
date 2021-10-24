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

#include <Python.h>

#ifndef PYJION_UNBOXEDRANGEOBJECT_H
#define PYJION_UNBOXEDRANGEOBJECT_H

typedef struct {
    // Copy of the Python type as its not exported.
    PyObject_HEAD
            PyObject* start;
    PyObject* stop;
    PyObject* step;
    PyObject* length;
} py_rangeobject;

typedef struct {
    PyObject_HEAD
            int64_t index;
    int64_t start;
    int64_t step;
    int64_t len;
} pyjion_rangeiterobject;

PyAPI_DATA(PyTypeObject) PyjionRangeIter_Type;
#define PyjionRangeIter_Check(op) Py_IS_TYPE(op, &PyjionRangeIter_Type)
#endif//PYJION_UNBOXEDRANGEOBJECT_H
