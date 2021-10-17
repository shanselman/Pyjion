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
    int64_t numDigits = 0;
    int64_t asLong();
    digit digits[1];
};

#define SIZEOF_BIGINT(n) ((sizeof(PyjionBigInt) - sizeof(PyjionBigInt::digits)) + (n * sizeof(digit)))


class PyjionBigIntRegister {
    std::vector<PyjionBigInt*> ints;
public:
    explicit PyjionBigIntRegister(size_t reserve = 0) {
        ints.reserve(reserve);
    }

    PyjionBigInt* addLong(long size) {
        PyjionBigInt* i = static_cast<PyjionBigInt*>(PyMem_Malloc(SIZEOF_BIGINT(size)));
        i->shortVersion = -1;
        i->isShort = false;
        i->numDigits = size;
        ints.emplace_back(i);
        return i;
    }
    PyjionBigInt* addShort(int64_t value) {
        PyjionBigInt* i = static_cast<PyjionBigInt*>(PyMem_Malloc(sizeof(PyjionBigInt)));
        i->shortVersion = value;
        i->isShort = true;
        ints.emplace_back(i);
        return i;
    }

    ~PyjionBigIntRegister() {
        for (auto i : ints) {
            PyMem_Free(i);
        }
    }
};

PyjionBigInt* PyjionBigInt_FromInt64(int64_t value, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_FromPyLong(PyObject* pythonObject, PyjionBigIntRegister* bigIntRegister);

PyjionBigInt* PyjionBigInt_Add(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_AddInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_AddInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister);

PyjionBigInt* PyjionBigInt_Sub(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_SubInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_SubInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister);

double PyjionBigInt_TrueDivide(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
double PyjionBigInt_TrueDivideInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
double PyjionBigInt_TrueDivideInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister);

PyjionBigInt* PyjionBigInt_Mod(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_ModInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_ModInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister);

PyjionBigInt* PyjionBigInt_Multiply(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_MultiplyInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_MultiplyInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister);

PyjionBigInt* PyjionBigInt_Power(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_PowerInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_PowerInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister);

PyjionBigInt* PyjionBigInt_FloorDivide(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_FloorDivideInt64Left(int64_t left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister);
PyjionBigInt* PyjionBigInt_FloorDivideInt64Right(PyjionBigInt* left, int64_t right, PyjionBigIntRegister* bigIntRegister);

int32_t PyjionBigInt_RichCompare(PyjionBigInt* left, PyjionBigInt* right, uint32_t type);
int32_t PyjionBigInt_RichCompareInt64Left(int64_t left, PyjionBigInt* right, uint32_t type);
int32_t PyjionBigInt_RichCompareInt64Right(PyjionBigInt* left, int64_t right, uint32_t type);

PyObject* PyjionBigInt_AsPyLong(PyjionBigInt*);
double PyjionBigInt_AsDouble(PyjionBigInt*);

#endif//PYJION_BIGINT_H
