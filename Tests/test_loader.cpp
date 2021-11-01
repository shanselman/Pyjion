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

#include <catch2/catch.hpp>
#include "peloader.h"
#include "corehost.h"
#include "error_codes.h"

TEST_CASE("Test basic loader") {
    SECTION("Test builtin R2R image") {
        PEDecoder decoder = PEDecoder("/usr/local/share/dotnet/shared/Microsoft.NETCore.App/6.0.0-rc.2.21480.5/System.Console.dll");
        CHECK(decoder.GetCorHeader()->Flags & COMIMAGE_FLAGS_IL_LIBRARY);
        CHECK(decoder.GetReadyToRunHeader()->MajorVersion == 5);
        CHECK(decoder.GetModuleName() == "System.Console.dll");
        auto typeRefs = decoder.GetTypeRefs();
        CHECK(typeRefs.size() == 125);
        CHECK(typeRefs[124].Name == 6055);
        auto typeDefs = decoder.GetTypeDefs();
        CHECK(typeDefs.size() == 53);
        CHECK(typeDefs[0].Flags == 0);
        CHECK(typeDefs[0].Name == 0x0577);
        CHECK(typeDefs[0].Namespace == 0);
        CHECK(typeDefs[0].Extends == 0);
        CHECK(typeDefs[0].FieldList == 1);
        CHECK(typeDefs[0].MethodList == 1);

        CHECK(typeDefs[1].Flags == 0x00100100);
        CHECK(typeDefs[1].Name == 0x1523);
        CHECK(typeDefs[1].Namespace == 0x2e74);
        CHECK(typeDefs[1].Extends == 5);
        CHECK(typeDefs[1].FieldList == 1);
        CHECK(typeDefs[1].MethodList == 1);
        // ...
        CHECK(typeDefs[40].Name == 2380);

        auto publicTypeDefs = decoder.GetPublicClasses();
        CHECK(publicTypeDefs.size() == 8);
        CHECK(decoder.GetString(publicTypeDefs[0].Name) == "Console");
        auto methods = decoder.GetClassMethods(publicTypeDefs[0]);
        CHECK(methods.size() == 59);
        CHECK(decoder.GetString(methods[0].Name) == "ReadKey");
        CHECK(decoder.GetString(methods[58].Name) == "Write");

        auto signature = decoder.GetBlob(methods[0].Signature);
        CHECK(signature.size() == 4);

        CHECK(load_hostfxr("/Users/anthonyshaw/CLionProjects/pyjion/CoreCLR/artifacts/bin/testhost/net6.0-OSX-Debug-x64/host/fxr/6.0.0/libhostfxr.dylib"));
        auto t = get_dotnet_load_assembly("/Users/anthonyshaw/CLionProjects/pyjion/CoreCLR/artifacts/bin/testhost/net6.0-OSX-Debug-x64/shared/Microsoft.NETCore.App/6.0.0/demo.runtimeconfig.json");
        REQUIRE(t != nullptr);

        component_entry_point_fn beep = nullptr;
        StatusCode ret = (StatusCode)t(
                "System.Runtime.Numerics.dll",
                "System.Numerics.BigInteger, System.Runtime.Numerics, Version=5.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a",
                "TryParse",
                nullptr /*delegate_type_name*/,
                nullptr,
                (void**) &beep);
        // </SnippetLoadAndGet>
        REQUIRE(ret == Success);
        REQUIRE(beep != nullptr);

        beep(nullptr, 0);
    }
}
