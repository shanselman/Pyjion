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


PyjionBigInt* PyjionBigInt_FromInt64(int64_t value, PyjionBigIntRegister* bigIntRegister) {
    auto result = new PyjionBigInt{
            .shortVersion = value,
            .isShort = true,
    };
    bigIntRegister->add(result);
    return result;
}

PyjionBigInt* PyjionBigInt_FromPyLong(PyObject* pythonObject, PyjionBigIntRegister* bigIntRegister) {
    PyjionBigInt* result;
    if (!IntegerValue::isBig(pythonObject)) {
        result = new PyjionBigInt{
                .shortVersion = PyLong_AsLongLong(pythonObject),
                .isShort = true,
        };
    } else {
        result = new PyjionBigInt{
                .shortVersion = -1,
                .isShort = false,
        };
        result->negative = (Py_SIZE(pythonObject) < 0);
        auto size = Py_ABS(Py_SIZE(pythonObject));
        for (size_t idx = 0; idx < size; idx++) {
            result->digits.push_back(((PyLongObject*) pythonObject)->ob_digit[idx]);
        }
    }
    bigIntRegister->add(result);
    return result;
}

PyjionBigInt* PyjionBigInt_Add(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(left->shortVersion + right->shortVersion, bigIntRegister);
    }
    PyjionBigInt* result = nullptr;
    if (left->isShort && !right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_add(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)), bigIntRegister);
    } else if (!left->isShort && right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_add(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)), bigIntRegister);
    } else {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_add(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)), bigIntRegister);
    }
    return result;
}

PyjionBigInt* PyjionBigInt_Sub(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(left->shortVersion - right->shortVersion, bigIntRegister);
    }
    PyjionBigInt* result = nullptr;
    if (left->isShort && !right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_subtract(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)), bigIntRegister);
    } else if (!left->isShort && right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_subtract(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)), bigIntRegister);
    } else {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_subtract(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)), bigIntRegister);
    }
    return result;
}

double PyjionBigInt_TrueDivide(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    double result = 0.;
    if (left->isShort && right->isShort) {
        result = PyJit_LongTrueDivide(left->shortVersion, right->shortVersion);
    } else if (left->isShort && !right->isShort) {
        result = PyFloat_AsDouble(PyLong_Type.tp_as_number->nb_true_divide(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)));
    } else if (!left->isShort && right->isShort) {
        result = PyFloat_AsDouble(PyLong_Type.tp_as_number->nb_true_divide(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)));
    } else {
        result = PyFloat_AsDouble(PyLong_Type.tp_as_number->nb_true_divide(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)));
    }
    return result;
}

PyjionBigInt* PyjionBigInt_Mod(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(PyJit_LongMod(left->shortVersion, right->shortVersion), bigIntRegister);
    }
    PyjionBigInt* result = nullptr;
    if (left->isShort && !right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_remainder(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)), bigIntRegister);
    } else if (!left->isShort && right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_remainder(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)), bigIntRegister);
    } else {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_remainder(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)), bigIntRegister);
    }
    return result;
}

PyjionBigInt* PyjionBigInt_Multiply(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        auto x = left->shortVersion * right->shortVersion;
        if (left->shortVersion != 0 && x / left->shortVersion != right->shortVersion) {
            // overflow handling
            return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_multiply(PyLong_FromLongLong(left->shortVersion), PyLong_FromLongLong(right->shortVersion)), bigIntRegister);
        } else {
            return PyjionBigInt_FromInt64(x, bigIntRegister);
        }
    }
    PyjionBigInt* result = nullptr;
    if (left->isShort && !right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_multiply(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)), bigIntRegister);
    } else if (!left->isShort && right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_multiply(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)), bigIntRegister);
    } else {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_multiply(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)), bigIntRegister);
    }
    return result;
}

PyjionBigInt* PyjionBigInt_Power(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        auto result = PyJit_LongPow(left->shortVersion, right->shortVersion);
        if (left->shortVersion != 0 && result != 0)
            return PyjionBigInt_FromInt64(result, bigIntRegister);
        else
            return PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_power(PyLong_FromLongLong(left->shortVersion), PyLong_FromLongLong(right->shortVersion), Py_None), bigIntRegister);
    }

    PyjionBigInt* result = nullptr;
    if (left->isShort && !right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_power(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right), Py_None), bigIntRegister);
    } else if (!left->isShort && right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_power(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion), Py_None), bigIntRegister);
    } else {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_power(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right), Py_None), bigIntRegister);
    }
    return result;
}

PyjionBigInt* PyjionBigInt_FloorDivide(PyjionBigInt* left, PyjionBigInt* right, PyjionBigIntRegister* bigIntRegister) {
    if (left->isShort && right->isShort) {
        return PyjionBigInt_FromInt64(PyJit_LongFloorDivide(left->shortVersion, right->shortVersion), bigIntRegister);
    }

    PyjionBigInt* result = nullptr;
    if (left->isShort && !right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_floor_divide(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right)), bigIntRegister);
    } else if (!left->isShort && right->isShort) {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_floor_divide(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion)), bigIntRegister);
    } else {
        result = PyjionBigInt_FromPyLong(PyLong_Type.tp_as_number->nb_floor_divide(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right)), bigIntRegister);
    }
    return result;
}

PyObject* PyjionBigInt_AsPyLong(PyjionBigInt* i) {
    PyObject* result = nullptr;
    if (i->isShort)
        result = PyLong_FromLongLong(i->shortVersion);
    else {
        auto l = _PyLong_New(i->digits.size());
        for (size_t idx = 0; idx < i->digits.size(); idx++) {
            l->ob_digit[idx] = i->digits[idx];
        }
        if (i->negative) {
            Py_SET_SIZE(l, -i->digits.size());
        }
        result = reinterpret_cast<PyObject*>(l);
    }
    return result;
}

int64_t PyjionBigInt::asLong() {
    if (isShort)
        return shortVersion;
    else
        return PyLong_AsLongLong(PyjionBigInt_AsPyLong(this));
}

int32_t PyjionBigInt_RichCompare(PyjionBigInt* left, PyjionBigInt* right, uint32_t op) {
    int result = -1;

    if (left->isShort && right->isShort)
        goto shortCompare;

    if (left->isShort && !right->isShort) {
        result = PyLong_Type.tp_richcompare(PyLong_FromLongLong(left->shortVersion), PyjionBigInt_AsPyLong(right), op) == Py_True ? 1 : 0;
    } else if (!left->isShort && right->isShort) {
        result = PyLong_Type.tp_richcompare(PyjionBigInt_AsPyLong(left), PyLong_FromLongLong(right->shortVersion), op) == Py_True ? 1 : 0;
    } else {
        result = PyLong_Type.tp_richcompare(PyjionBigInt_AsPyLong(left), PyjionBigInt_AsPyLong(right), op) == Py_True ? 1 : 0;
    }
    goto cleanup;

shortCompare:
    switch (op) {
        case Py_EQ:
            result = left->shortVersion == right->shortVersion;
            break;
        case Py_NE:
            result = left->shortVersion != right->shortVersion;
            break;
        case Py_GE:
            result = left->shortVersion >= right->shortVersion;
            break;
        case Py_LE:
            result = left->shortVersion <= right->shortVersion;
            break;
        case Py_LT:
            result = left->shortVersion < right->shortVersion;
            break;
        case Py_GT:
            result = left->shortVersion > right->shortVersion;
            break;
        default:
            result = -1;
    }

cleanup:
    return result;
}