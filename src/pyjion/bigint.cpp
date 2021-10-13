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

#include "bigint.h"


PyjionBigInt* PyjionBigInt_FromInt64(int64_t value){
    return new PyjionBigInt{
        .shortVersion = value,
        .isShort =  true,
        .pythonObject = nullptr
    };
}

PyjionBigInt* PyjionBigInt_FromPyLong(PyObject* pythonObject){
    return new PyjionBigInt{
            .shortVersion = 0,
            .isShort = false,
            .pythonObject = pythonObject
    };
}

PyjionBigInt* PyjionBigInt_Add(PyjionBigInt* left, PyjionBigInt* right){
    left->shortVersion += right->shortVersion; // TODO : Check size and shift
    return left;
}

PyObject* PyjionBigInt_AsPyLong(PyjionBigInt*i){
    if (i->isShort)
        return PyLong_FromLongLong(i->shortVersion);
    else
        return i->pythonObject;
}