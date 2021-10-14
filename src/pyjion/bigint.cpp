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
#include "intrins.h"


PyjionBigInt* PyjionBigInt_FromInt64(int64_t value) {
    return new PyjionBigInt{
            .shortVersion = value,
            .isShort = true,
    };
}

PyjionBigInt* PyjionBigInt_FromPyLong(PyObject* pythonObject) {
    if (!IntegerValue::isBig(pythonObject)) {// TODO : Work out exact bits
        return new PyjionBigInt{
                .shortVersion = PyLong_AsLongLong(pythonObject),
                .isShort = true,
        };
    } else {
        auto i = new PyjionBigInt{
                .shortVersion = -1,
                .isShort = false,
        };
        i->negative = (Py_SIZE(pythonObject) < 0);
        auto size = Py_ABS(Py_SIZE(pythonObject));
        for (size_t idx = 0; idx < size; idx++) {
            i->digits.push_back(((PyLongObject*) pythonObject)->ob_digit[idx]);
        }
        return i;
    }
}

PyjionBigInt* PyjionBigInt_Add(PyjionBigInt* left, PyjionBigInt* right) {
    if (left->isShort && right->isShort) {
        left->shortVersion += right->shortVersion;
        return left;
    } else if (left->isShort && !right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_add(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)));
    } else if (!left->isShort && right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_add(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)));
    } else {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_add(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)));
    }
}

PyjionBigInt* PyjionBigInt_Sub(PyjionBigInt* left, PyjionBigInt* right) {
    if (left->isShort && right->isShort) {
        left->shortVersion -= right->shortVersion;
        return left;
    } else if (left->isShort && !right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_subtract(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)));
    } else if (!left->isShort && right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_subtract(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)));
    } else {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_subtract(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)));
    }
}

double PyjionBigInt_TrueDivide(PyjionBigInt* left, PyjionBigInt* right) {
    if (left->isShort && right->isShort) {
        return PyJit_LongTrueDivide(left->shortVersion, right->shortVersion);
    } else if (left->isShort && !right->isShort) {
        return PyFloat_AsDouble(PyLong_Type.tp_as_number->nb_true_divide(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)));
    } else if (!left->isShort && right->isShort) {
        return PyFloat_AsDouble(PyLong_Type.tp_as_number->nb_true_divide(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)));
    } else {
        return PyFloat_AsDouble(PyLong_Type.tp_as_number->nb_true_divide(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)));
    }
}

PyjionBigInt* PyjionBigInt_Mod(PyjionBigInt* left, PyjionBigInt* right) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(PyJit_LongMod(left->shortVersion, right->shortVersion));
    } else if (left->isShort && !right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_remainder(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)));
    } else if (!left->isShort && right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_remainder(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)));
    } else {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_remainder(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)));
    }
}

PyjionBigInt* PyjionBigInt_Multiply(PyjionBigInt* left, PyjionBigInt* right) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(left->shortVersion * right->shortVersion);
    } else if (left->isShort && !right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_multiply(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)));
    } else if (!left->isShort && right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_multiply(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)));
    } else {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_multiply(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)));
    }
}

PyjionBigInt* PyjionBigInt_Power(PyjionBigInt* left, PyjionBigInt* right) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(PyJit_LongPow(left->shortVersion, right->shortVersion));// TODO : Check overflow!!!
    } else if (left->isShort && !right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_power(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right), Py_None));
    } else if (!left->isShort && right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_power(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion), Py_None));
    } else {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_power(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right), Py_None));
    }
}

PyjionBigInt* PyjionBigInt_FloorDivide(PyjionBigInt* left, PyjionBigInt* right) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(PyJit_LongFloorDivide(left->shortVersion, right->shortVersion));
    } else if (left->isShort && !right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_floor_divide(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)));
    } else if (!left->isShort && right->isShort) {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_floor_divide(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)));
    } else {
        return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_floor_divide(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)));
    }
}

PyObject* PyjionBigInt_AsPyLong(PyjionBigInt* i) {
    if (i->isShort)
        return PyLong_FromLongLong(i->shortVersion);
    else {
        auto l = _PyLong_New(i->digits.size());
        for (size_t idx = 0; idx < i->digits.size(); idx++) {
            l->ob_digit[idx] = i->digits[idx];
        }
        if (i->negative) {
            Py_SET_SIZE(l, -i->digits.size());
        }
        return reinterpret_cast<PyObject*>(l);
    }
}

int64_t PyjionBigInt::asLong() {
    if (isShort)
        return shortVersion;
    else
        return PyLong_AsLongLong(PyjionBigInt_AsPyLong(this));
}

int32_t PyjionBigInt_RichCompare(PyjionBigInt* left, PyjionBigInt* right, uint32_t op) {
    if (left->isShort && right->isShort)
        goto shortCompare;

    if (left->isShort && !right->isShort) {
        return PyLong_Type.tp_richcompare(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right), op) == Py_True ? 1 : 0;
    } else if (!left->isShort && right->isShort) {
        return PyLong_Type.tp_richcompare(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion), op) == Py_True ? 1 : 0;
    } else {
        return PyLong_Type.tp_richcompare(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right), op) == Py_True ? 1 : 0;
    }

shortCompare:
    switch (op) {
        case Py_EQ:
            return left->shortVersion == right->shortVersion;
        case Py_NE:
            return left->shortVersion != right->shortVersion;
        case Py_GE:
            return left->shortVersion >= right->shortVersion;
        case Py_LE:
            return left->shortVersion <= right->shortVersion;
        case Py_LT:
            return left->shortVersion < right->shortVersion;
        case Py_GT:
            return left->shortVersion > right->shortVersion;
        default:
            return -1;
    }
}