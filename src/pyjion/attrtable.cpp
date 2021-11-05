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

#include "attrtable.h"

int AttributeTable::captureStoreAttr(PyTypeObject* ty, const char* name, AbstractValueKind kind) {
#ifdef DEBUG_VERBOSE
    printf("Capturing value of %s.%s is %u\n", ty->tp_name, name, kind);
#endif
    if (table.find(ty) == table.end()){
        table[ty] = unordered_map<const char*, AbstractValueKind>();
        table[ty][name] = kind;
    } else {
        if (table[ty].find(name) == table[ty].end()){
            table[ty][name] = kind;
        } else {
            if (table[ty][name] == kind)
                return 0;
            switch (table[ty][name]){
                case AVK_Any:
                    //Already a bad value
                    break;
                case AVK_None:
                    table[ty][name] = kind;
                    break;
                default:
                    // Mark as variable type...
                    table[ty][name] = AVK_Any;
                    return -1;
            }
        }
    }
    return 0;
}
AbstractValueKind AttributeTable::getAttr(PyTypeObject* ty, const char* name){
    if (table.find(ty) == table.end()){
        return AVK_Any;
    } else {
        if (table[ty].find(name) == table[ty].end()) {
            return AVK_Any;
        } else {
            return table[ty][name];
        }
    }
}