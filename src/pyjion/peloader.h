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

using namespace std;

enum class AddressType {
    PhysicalOffset,
    RelativeVirtualAddress,
    VirtualAddress
};


int printExps(void *N,
              const peparse::VA &funcAddr,
              const std::string &mod,
              const std::string &func);

int printImports(void *N,
                 const peparse::VA &impAddr,
                 const std::string &modName,
                 const std::string &symName) ;

int printSymbols(void *N,
                 const std::string &strName,
                 const uint32_t &value,
                 const int16_t &sectionNumber,
                 const uint16_t &type,
                 const uint8_t &storageClass,
                 const uint8_t &numberOfAuxSymbols) ;


int printRsrc(void *N, const peparse::resource &r);

int printSecs(void *N,
              const peparse::VA &secBase,
              const std::string &secName,
              const peparse::image_section_header &s,
              const peparse::bounded_buffer *data);

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

public:
    PEDecoder(const char* filePath) {
        errno = 0;
        pe = peparse::ParsePEFromFile(filePath);
        if (!pe) {
            // TODO: Get PE Err.
            throw InvalidImageException("Failed to load image. Invalid DLL?");
        }
        vector<uint8_t> data;// TODO pre-alloc for known size.
        if (GetDataDirectoryEntry(pe, peparse::data_directory_kind::DIR_COM_DESCRIPTOR, data)) {
            memcpy(&corHeader, data.data(), sizeof(IMAGE_COR20_HEADER));
        } else {
            throw InvalidImageException("Failed to load image. Not a .NET DLL");
        }
        NativeFormat::NativeReader m_nativeReader = NativeFormat::NativeReader((PTR_CBYTE) pe->fileBuffer->buf, pe->peHeader.nt.OptionalHeader64.SizeOfImage);

        uint64_t offsetOfManagedHeader;
        if (convertAddress(pe, corHeader.ManagedNativeHeader.VirtualAddress, AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, offsetOfManagedHeader)) {
            memcpy(&r2rHeader, pe->fileBuffer->buf + offsetOfManagedHeader, sizeof(READYTORUN_HEADER));
            READYTORUN_SECTION sections[r2rHeader.CoreHeader.NumberOfSections];

            for (size_t i = 0; i < r2rHeader.CoreHeader.NumberOfSections; i++) {
                memcpy(&sections[i], pe->fileBuffer->buf + offsetOfManagedHeader + sizeof(READYTORUN_HEADER) + (i * sizeof(READYTORUN_SECTION)), sizeof(READYTORUN_HEADER));
                switch (sections[i].Type) {
                    case ReadyToRunSectionType::CompilerIdentifier: {
                        uint64_t offsetOfCompilerIdentifier;
                        if (!convertAddress(pe, sections[i].Section.VirtualAddress, AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, offsetOfCompilerIdentifier)) {
                            throw InvalidImageException("Failed to load image. Corrupt section table.");
                        }
                        strncpy(compilerIdentifier, reinterpret_cast<const char*>(pe->fileBuffer->buf + offsetOfCompilerIdentifier), sections[i].Section.Size <= COMPILER_ID_SIZE ? sections[i].Section.Size : COMPILER_ID_SIZE);
                    } break;
                    case ReadyToRunSectionType::ImportSections: {
                        READYTORUN_IMPORT_SECTION importSections[sections[i].Section.Size / sizeof(READYTORUN_IMPORT_SECTION)];
                        uint64_t offset;
                        if (!convertAddress(pe, sections[i].Section.VirtualAddress, AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, offset)) {
                            throw InvalidImageException("Failed to load image. Corrupt section table.");
                        }
                        memcpy(importSections, pe->fileBuffer->buf + offset, sections[i].Section.Size);
                        // Map import sections.
                        for(int j = 0; j < sections[i].Section.Size / sizeof(READYTORUN_IMPORT_SECTION); j++){
                            allImportSections.push_back(importSections[j]);
                        }
                    } break;
                    case ReadyToRunSectionType::RuntimeFunctions: {
                        RUNTIME_FUNCTION runtimeFunctions[sections[i].Section.Size / sizeof(RUNTIME_FUNCTION)];
                        uint64_t offset;
                        if (!convertAddress(pe, sections[i].Section.VirtualAddress, AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, offset)) {
                            throw InvalidImageException("Failed to load image. Corrupt section table.");
                        }
                        memcpy(runtimeFunctions, pe->fileBuffer->buf + offset, sections[i].Section.Size);
                        // Map import sections.
                        for(int j = 0; j < sections[i].Section.Size / sizeof(RUNTIME_FUNCTION); j++){
                            allRuntimeFunctions.push_back(runtimeFunctions[j]);
                        }
                    } break;
                    case ReadyToRunSectionType::MethodDefEntryPoints: {
                        uint64_t offset;
                        if (!convertAddress(pe, sections[i].Section.VirtualAddress, AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, offset)) {
                            throw InvalidImageException("Failed to load image. Corrupt section table.");
                        }
                        m_methodDefEntryPoints = NativeFormat::NativeArray(&m_nativeReader, offset);
                    }
                        break;
                }
            }
        } else {
            throw InvalidImageException("Failed to load image. Corrupt section table.");
        }

        // Run import sections to get fixups and signatures..

        for (auto & section : allImportSections){
            if (section.Signatures) {
                uint64_t sigOffset;
                if (!convertAddress(pe, section.Signatures, AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, sigOffset)) {
                    throw InvalidImageException("Failed to load image. Corrupt section table.");
                }

                uint64_t fixupsOffset;
                if (!convertAddress(pe, section.Section.VirtualAddress, AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, fixupsOffset)) {
                    throw InvalidImageException("Failed to load image. Corrupt section table.");
                }

                PVOID *pFixups = (PVOID *)(DWORD *)(pe->fileBuffer->buf + fixupsOffset);
                DWORD nFixups = section.Section.Size / sizeof(void*);
                DWORD * signatures = (DWORD *)(pe->fileBuffer->buf + sigOffset);
                for (DWORD i = 0; i < nFixups; i++)
                {
                    uint64_t pOffset;
                    convertAddress(pe, signatures[i], AddressType::RelativeVirtualAddress, AddressType::PhysicalOffset, pOffset);
                    PBYTE pSig = (PBYTE)(pe->fileBuffer->buf + pOffset);
                    //printf("0x%02X - %d\n", pSig[0], pSig[1]);
                }
            }
        }
    }
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
};


#endif//PYJION_PELOADER_H
