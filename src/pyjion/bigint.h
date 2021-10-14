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
#include <vector>

struct PyjionBigInt {
    int64_t shortVersion = -1;
    bool isShort;
    bool negative;// Only used in long integers.
    std::vector<digit> digits;

    int64_t asLong();
};

class PyjionBigIntRegister {
    std::vector<PyjionBigInt*> ints;

public:
    void add(PyjionBigInt* i) {
        ints.push_back(i);
    }
    ~PyjionBigIntRegister() {
        for (auto i : ints) {
            delete i;
        }
    }
};

PyjionBigInt* PyjionBigInt_FromInt64(int64_t value, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_FromPyLong(PyObject* pythonObject, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_Add(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_Sub(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
double PyjionBigInt_TrueDivide(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_Mod(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_Multiply(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_Power(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_FloorDivide(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
int32_t PyjionBigInt_RichCompare(PyjionBigInt* left, PyjionBigInt* right, uint32_t type);

PyObject* PyjionBigInt_AsPyLong(PyjionBigInt*);

#endif//PYJION_BIGINT_H
