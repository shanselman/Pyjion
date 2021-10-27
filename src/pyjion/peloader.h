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

#include <pe-parse/parse.h>

#include <windows.h>
#include <corhdr.h>
#include <readytorun.h>
#include <holder.h>
#include <vector>

#include <iostream>
#include <limits>
#include <memory>

#include <climits>
#include <cstring>
#include "exceptions.h"

#define T_CONTEXT CONTEXT
#define T_RUNTIME_FUNCTION RUNTIME_FUNCTION
#include <daccess.h>
#define EEPOLICY_HANDLE_FATAL_ERROR(error) assert(!"EEPOLICY_HANDLE_FATAL_ERROR")
#include <nativeformatreader.h>

#ifndef PYJION_PELOADER_H
#define PYJION_PELOADER_H
#define ALIGN4BYTE(val) (((val) + 3) & ~0x3)

using namespace std;

enum class AddressType {
    PhysicalOffset,
    RelativeVirtualAddress,
    VirtualAddress
};

struct ECMA_STORAGE_HEADER {
    uint32_t      lSignature;             // "Magic" signature.
    uint16_t      iMajorVer;              // Major file version.
    uint16_t      iMinorVer;              // Minor file version.
    uint32_t      iExtraData;             // Offset to next structure of information
    uint32_t      iVersionString;         // Length of version string
    BYTE          pVersion[0];            // Version string

};
struct ECMA_STORAGE_HEADER2 {
    BYTE fFlags;// STGHDR_xxx flags.
    BYTE pad;
    uint16_t iStreams;// How many streams are there.
};

struct ECMA_STREAM_HEADER {
    uint32_t offset;     // Memory offset to start of this stream from start of the metadata root
    uint32_t size;       // Size of this stream in bytes, shall be a multiple of 4.
    char     name[32];   // Stream name
};

struct HOME_TABLE_HEADER { // Header for #~
    uint32_t Reserved1; // Reserved, always 0
    BYTE MajorVersion; // Major version of table schemata, always 1
    BYTE MinorVersion; // Minor version of table schemata, always 0
    BYTE HeapOffsetSizes; // Bit vector for heap sizes.
    BYTE Reserved2; // Reserved, always 1
    bitset<64> MaskValid; // Bit vector of present tables, let n be the number of bits that are 1.
    bitset<64> MaskSorted; // Bit vector of present tables, let n be the number of bits that are 1.
};

enum MetaDataTable
{
    Module = 0x00,
    TypeRef = 0x01,
    TypeDef = 0x02,
    FieldPtr = 0x03,
    Field = 0x04,
    MethodPtr = 0x05,
    Method = 0x06,
    ParamPtr = 0x07,
    Param = 0x08,
    InterfaceImpl = 0x09,
    MemberRef = 0x0A,
    Constant = 0x0B,
    CustomAttribute = 0x0C,
    FieldMarshal = 0x0D,
    DeclSecurity = 0x0E,
    ClassLayout = 0x0F,
    FieldLayout = 0x10,
    StandAloneSig = 0x11,
    EventMap = 0x12,
    EventPtr = 0x13,
    Event = 0x14,
    PropertyMap = 0x15,
    PropertyPtr = 0x16,
    Property = 0x17,
    MethodSemantics = 0x18,
    MethodImpl = 0x19,
    ModuleRef = 0x1A,
    TypeSpec= 0x1B,
    ImplMap= 0x1C,
    FieldRVA= 0x1D,
    ENCLog= 0x1E,
    ENCMap= 0x1F,
    Assembly= 0x20,
    AssemblyProcessor= 0x21,
    AssemblyOS= 0x22,
    AssemblyRef= 0x23,
    AssemblyRefProcessor= 0x24,
    AssemblyRefOS= 0x25,
    File= 0x26,
    ExportedType= 0x27,
    ManifestResource= 0x28,
    NestedClass= 0x29,
    GenericParam= 0x2A,
    MethodSpec= 0x2B,
    GenericParamConstraint= 0x2C,
};

struct ModuleTableRow {
    uint16_t Generation;
    uint16_t Name;
    uint16_t Mvid;
    uint16_t EncId;
    uint16_t EncBaseId;
};

static const MetaDataTable AllMetaDataTables[] = { Module, TypeRef, TypeDef, FieldPtr, Field, MethodPtr, Method, ParamPtr, Param, InterfaceImpl, MemberRef, Constant, CustomAttribute, FieldMarshal, DeclSecurity, ClassLayout, FieldLayout, StandAloneSig, EventMap, EventPtr, Event, PropertyMap, PropertyPtr, Property, MethodSemantics, MethodImpl, ModuleRef, TypeSpec, ImplMap, FieldRVA, ENCLog, ENCMap, Assembly, AssemblyProcessor, AssemblyOS, AssemblyRef, AssemblyRefProcessor, AssemblyRefOS, File, ExportedType, ManifestResource, NestedClass, GenericParam, MethodSpec, GenericParamConstraint };

bool convertAddress(peparse::parsed_pe* pe,
                    std::uint64_t address,
                    AddressType source_type,
                    AddressType destination_type,
                    std::uint64_t& result);

class InvalidImageException : public PyjionJitRuntimeException {
private:
    const char* reason = "InvalidImageException";

public:
    explicit InvalidImageException(const char* why) : PyjionJitRuntimeException() {
        reason = why;
    };
    const char* what() const noexcept override {
        return reason;
    }
};

#define COMPILER_ID_SIZE 50

class PEDecoder {
private:
    peparse::parsed_pe* pe;
    IMAGE_COR20_HEADER corHeader;
    READYTORUN_HEADER r2rHeader;
    char compilerIdentifier[COMPILER_ID_SIZE] = "";
    vector<READYTORUN_IMPORT_SECTION> allImportSections;
    vector<RUNTIME_FUNCTION> allRuntimeFunctions;
    NativeFormat::NativeArray m_methodDefEntryPoints;
    vector<ECMA_STREAM_HEADER> streamHeaders;
    map<MetaDataTable, uint32_t> metaDataTableSizes;
    vector<ModuleTableRow> moduleTable;
    std::string stringHeap;

public:
    PEDecoder(const char* filePath);
    ~PEDecoder() {
        peparse::DestructParsedPE(pe);
    }

    IMAGE_COR20_HEADER* GetCorHeader() {
        return &corHeader;
    }

    bool IsReadyToRun() const {
        return corHeader.Flags & COMIMAGE_FLAGS_IL_LIBRARY;
    }

    READYTORUN_HEADER* GetReadyToRunHeader() {
        return &r2rHeader;
    }

    std::string GetString(uint64_t offset){ // TODO : Optimize
        uint64_t i = offset;
        std::string result;
        while (stringHeap[i] != '\0'){
            result.push_back(stringHeap[i]);
            i++;
        }
        return result;
    }

    std::string GetModuleName(){
        if (moduleTable.size() != 1)
            return "";
        return GetString(moduleTable[0].Name);
    }
};


#endif//PYJION_PELOADER_H
