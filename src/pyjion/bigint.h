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

#ifndef PYJION_BIGINT_H
#define PYJION_BIGINT_H

#include <cstdint>
#include <Python.h>


struct PyjionBigInt {
    int64_t shortVersion;
    bool isShort;
    PyObject* pythonObject;

    int64_t asLong() const {
        if (isShort)
            return shortVersion;
        else
            return PyLong_AsLongLong(pythonObject);
    }
};

PyjionBigInt* PyjionBigInt_FromInt64(int64_t value);
PyjionBigInt* PyjionBigInt_FromPyLong(PyObject* pythonObject);
PyjionBigInt* PyjionBigInt_Add(PyjionBigInt* left, PyjionBigInt*  right);
PyObject* PyjionBigInt_AsPyLong(PyjionBigInt*);

#endif //PYJION_BIGINT_H
