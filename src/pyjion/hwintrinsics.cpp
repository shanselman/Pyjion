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

#include "hwintrinsics.h"

const char* namespaceForIntrinsic(NamedIntrinsic intrinsic){
    switch (intrinsic){
        case NI_System_Math_Abs:
        case NI_System_Math_Acos:
        case    NI_System_Math_Acosh:
        case NI_System_Math_Asin:
        case         NI_System_Math_Asinh:
        case         NI_System_Math_Atan:
        case         NI_System_Math_Atanh:
        case         NI_System_Math_Atan2:
        case         NI_System_Math_Cbrt:
        case         NI_System_Math_Ceiling:
        case         NI_System_Math_Cos:
        case         NI_System_Math_Cosh:
        case         NI_System_Math_Exp:
        case         NI_System_Math_Floor:
        case         NI_System_Math_FMod:
        case         NI_System_Math_FusedMultiplyAdd:
        case         NI_System_Math_ILogB:
        case         NI_System_Math_Log:
        case         NI_System_Math_Log2:
        case         NI_System_Math_Log10:
        case         NI_System_Math_Pow:
        case         NI_System_Math_Round:
        case         NI_System_Math_Sin:
        case         NI_System_Math_Sinh:
        case         NI_System_Math_Sqrt:
        case         NI_System_Math_Tan:
        case         NI_System_Math_Tanh:
            return "System";
        default:
            return "System.Runtime.Intrinsics";
    }
}

const char* classnameForIntrinsic(NamedIntrinsic intrinsic) {
    switch (intrinsic){
        case NI_System_Math_Abs:
        case NI_System_Math_Acos:
        case    NI_System_Math_Acosh:
        case NI_System_Math_Asin:
        case         NI_System_Math_Asinh:
        case         NI_System_Math_Atan:
        case         NI_System_Math_Atanh:
        case         NI_System_Math_Atan2:
        case         NI_System_Math_Cbrt:
        case         NI_System_Math_Ceiling:
        case         NI_System_Math_Cos:
        case         NI_System_Math_Cosh:
        case         NI_System_Math_Exp:
        case         NI_System_Math_Floor:
        case         NI_System_Math_FMod:
        case         NI_System_Math_FusedMultiplyAdd:
        case         NI_System_Math_ILogB:
        case         NI_System_Math_Log:
        case         NI_System_Math_Log2:
        case         NI_System_Math_Log10:
        case         NI_System_Math_Pow:
        case         NI_System_Math_Round:
        case         NI_System_Math_Sin:
        case         NI_System_Math_Sinh:
        case         NI_System_Math_Sqrt:
        case         NI_System_Math_Tan:
        case         NI_System_Math_Tanh:
            return "Math";
        default:
            return "System.Runtime.Intrinsics";
    }
}

const char* methodForIntrinsic(NamedIntrinsic intrinsic){
    switch (intrinsic){
        case NI_System_Math_Abs:
        case NI_System_Math_Acos:
        case    NI_System_Math_Acosh:
        case NI_System_Math_Asin:
        case         NI_System_Math_Asinh:
        case         NI_System_Math_Atan:
        case         NI_System_Math_Atanh:
        case         NI_System_Math_Atan2:
        case         NI_System_Math_Cbrt:
        case         NI_System_Math_Ceiling:
        case         NI_System_Math_Cos:
        case         NI_System_Math_Cosh:
        case         NI_System_Math_Exp:
        case         NI_System_Math_Floor:
        case         NI_System_Math_FMod:
        case         NI_System_Math_FusedMultiplyAdd:
        case         NI_System_Math_ILogB:
        case         NI_System_Math_Log:
        case         NI_System_Math_Log2:
        case         NI_System_Math_Log10:
        case         NI_System_Math_Pow:
        case         NI_System_Math_Round:
        case         NI_System_Math_Sin:
        case         NI_System_Math_Sinh:
        case         NI_System_Math_Sqrt:
        case         NI_System_Math_Tan:
        case         NI_System_Math_Tanh:
            return "Sin";
        default:
            return "System.Runtime.Intrinsics";
    }
}
