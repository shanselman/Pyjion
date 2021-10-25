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
#include <fstream>
#include <holder.h>
#include <vector>

#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>

#include <climits>
#include <cstring>
#include "exceptions.h"

#ifndef PYJION_PELOADER_H
#define PYJION_PELOADER_H

using namespace std;

enum class AddressType {
    PhysicalOffset,
    RelativeVirtualAddress,
    VirtualAddress
};

bool convertAddress(peparse::parsed_pe *pe,
                    std::uint64_t address,
                    AddressType source_type,
                    AddressType destination_type,
                    std::uint64_t &result);

class InvalidImageException : public PyjionJitRuntimeException {
private:
    const char* reason = "InvalidImageException";
public:
    explicit InvalidImageException(const char* why) : PyjionJitRuntimeException()
    {
        reason = why;
    };
    const char* what() const noexcept override {
        return reason;
    }
};

class PEDecoder
{
private:
    peparse::parsed_pe *pe;
    IMAGE_COR20_HEADER corHeader;
    READYTORUN_HEADER r2rHeader;
    char compilerIdentifier[255];
    vector<READYTORUN_IMPORT_SECTION> allImportSections;
public:
    PEDecoder(const filesystem::path& filePath)
    {
        errno = 0;
        pe = peparse::ParsePEFromFile(filePath.c_str());
        if (!pe) {
            // TODO: Get PE Err.
            throw InvalidImageException("Failed to load image. Invalid DLL?");
        }
        vector<uint8_t> data; // TODO pre-alloc for known size.
        if (GetDataDirectoryEntry(pe, peparse::data_directory_kind::DIR_COM_DESCRIPTOR, data)){
            memcpy(&corHeader, data.data(), sizeof(IMAGE_COR20_HEADER));
        } else {
            throw InvalidImageException("Failed to load image. Not a .NET DLL");
        }
        uint64_t offsetOfManagedHeader ;
        if (convertAddress(pe, corHeader.ManagedNativeHeader.VirtualAddress, AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, offsetOfManagedHeader)) {
            memcpy(&r2rHeader, pe->fileBuffer->buf + offsetOfManagedHeader, sizeof(READYTORUN_HEADER));
            READYTORUN_SECTION sections[r2rHeader.CoreHeader.NumberOfSections];

            for (size_t i = 0 ; i < r2rHeader.CoreHeader.NumberOfSections ; i++){
                memcpy(&sections[i], pe->fileBuffer->buf + offsetOfManagedHeader + sizeof(READYTORUN_HEADER) + (i * sizeof(READYTORUN_SECTION)), sizeof(READYTORUN_HEADER));
                switch(sections[i].Type) {
                    case ReadyToRunSectionType::CompilerIdentifier : {
                        uint64_t offsetOfCompilerIdentifier ;
                        if (!convertAddress(pe, sections[i].Section.VirtualAddress, AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, offsetOfCompilerIdentifier)){
                            throw InvalidImageException("Failed to load image. Corrupt section table.");
                        }
                        memcpy(&compilerIdentifier, pe->fileBuffer->buf + offsetOfCompilerIdentifier, sections[i].Section.Size);
                    }
                        break;
                    case ReadyToRunSectionType::ImportSections : {
                        READYTORUN_IMPORT_SECTION importSections[sections[i].Section.Size / sizeof(READYTORUN_IMPORT_SECTION)];
                        uint64_t offset;
                        if (!convertAddress(pe, sections[i].Section.VirtualAddress, AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, offset)){
                            throw InvalidImageException("Failed to load image. Corrupt section table.");
                        }
                        memcpy(importSections, pe->fileBuffer->buf + offset, sections[i].Section.Size);
                        // Map import sections.
                    }
                        break;
                }

            }
        } else {
            throw InvalidImageException("Failed to load image. Corrupt section table.");
        }
    }
    ~PEDecoder(){
        peparse::DestructParsedPE(pe);
    }

    IMAGE_COR20_HEADER*  GetCorHeader(){
        return &corHeader;
    }

    bool IsReadyToRun() const{
        return corHeader.Flags & COMIMAGE_FLAGS_IL_LIBRARY;
    }

    READYTORUN_HEADER* GetReadyToRunHeader(){
        return &r2rHeader;
    }
};


#endif//PYJION_PELOADER_H
