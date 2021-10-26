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

#include "peloader.h"

bool convertAddress(peparse::parsed_pe * pe,
                    std::uint64_t address,
                    AddressType source_type,
                    AddressType destination_type,
                    std::uint64_t &result) {
    if (source_type == destination_type) {
        result = address;
        return true;
    }

    std::uint64_t image_base_address = 0U;
    if (pe->peHeader.nt.FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) {
        image_base_address = pe->peHeader.nt.OptionalHeader64.ImageBase;
    } else {
        image_base_address = pe->peHeader.nt.OptionalHeader.ImageBase;
    }

    struct SectionAddressLimits final {
        std::uintptr_t lowest_rva;
        std::uintptr_t lowest_offset;

        std::uintptr_t highest_rva;
        std::uintptr_t highest_offset;
    };

    auto L_getSectionAddressLimits =
            [](void *N,
               const peparse::VA &secBase,
               const std::string &secName,
               const peparse::image_section_header &s,
               const peparse::bounded_buffer *data) -> int {
        static_cast<void>(secBase);
        static_cast<void>(secName);
        static_cast<void>(data);

        SectionAddressLimits *section_address_limits =
                static_cast<SectionAddressLimits *>(N);

        section_address_limits->lowest_rva =
                std::min(section_address_limits->lowest_rva,
                         static_cast<std::uintptr_t>(s.VirtualAddress));

        section_address_limits->lowest_offset =
                std::min(section_address_limits->lowest_offset,
                         static_cast<std::uintptr_t>(s.PointerToRawData));

        std::uintptr_t sectionSize;
        if (s.SizeOfRawData != 0) {
            sectionSize = s.SizeOfRawData;
        } else {
            sectionSize = s.Misc.VirtualSize;
        }

        section_address_limits->highest_rva =
                std::max(section_address_limits->highest_rva,
                         static_cast<std::uintptr_t>(s.VirtualAddress + sectionSize));

        section_address_limits->highest_offset =
                std::max(section_address_limits->highest_offset,
                         static_cast<std::uintptr_t>(s.PointerToRawData + sectionSize));

        return 0;
    };

    SectionAddressLimits section_address_limits = {
            std::numeric_limits<std::uintptr_t>::max(),
            std::numeric_limits<std::uintptr_t>::max(),
            std::numeric_limits<std::uintptr_t>::min(),
            std::numeric_limits<std::uintptr_t>::min()};

    IterSec(pe, L_getSectionAddressLimits, &section_address_limits);

    switch (source_type) {
        case AddressType::PhysicalOffset: {
            if (address >= section_address_limits.highest_offset) {
                return false;
            }

            if (destination_type == AddressType::RelativeVirtualAddress) {
                struct CallbackData final {
                    bool found;
                    std::uint64_t address;
                    std::uint64_t result;
                };

                auto L_inspectSection = [](void *N,
                                           const peparse::VA &secBase,
                                           const std::string &secName,
                                           const peparse::image_section_header &s,
                                           const peparse::bounded_buffer *data) -> int {
                    static_cast<void>(secBase);
                    static_cast<void>(secName);
                    static_cast<void>(data);

                    std::uintptr_t sectionBaseOffset = s.PointerToRawData;

                    std::uintptr_t sectionEndOffset = sectionBaseOffset;
                    if (s.SizeOfRawData != 0) {
                        sectionEndOffset += s.SizeOfRawData;
                    } else {
                        sectionEndOffset += s.Misc.VirtualSize;
                    }

                    auto callback_data = static_cast<CallbackData *>(N);
                    if (callback_data->address >= sectionBaseOffset &&
                        callback_data->address < sectionEndOffset) {
                        callback_data->result = s.VirtualAddress + (callback_data->address -
                                                                    s.PointerToRawData);

                        callback_data->found = true;
                        return 1;
                    }

                    return 0;
                };

                CallbackData callback_data = {false, address, 0U};
                IterSec(pe, L_inspectSection, &callback_data);

                if (!callback_data.found) {
                    return false;
                }

                result = callback_data.result;
                return true;

            } else if (destination_type == AddressType::VirtualAddress) {
                std::uint64_t rva = 0U;
                if (!convertAddress(pe,
                                    address,
                                    source_type,
                                    AddressType::RelativeVirtualAddress,
                                    rva)) {
                    return false;
                }

                result = image_base_address + rva;
                return true;
            }

            return false;
        }

        case AddressType::RelativeVirtualAddress: {
            if (address < section_address_limits.lowest_rva) {
                result = address;
                return true;
            } else if (address >= section_address_limits.highest_rva) {
                return false;
            }

            if (destination_type == AddressType::PhysicalOffset) {
                struct CallbackData final {
                    bool found;
                    std::uint64_t address;
                    std::uint64_t result;
                };

                auto L_inspectSection = [](void *N,
                                           const peparse::VA &secBase,
                                           const std::string &secName,
                                           const peparse::image_section_header &s,
                                           const peparse::bounded_buffer *data) -> int {
                    static_cast<void>(secBase);
                    static_cast<void>(secName);
                    static_cast<void>(data);

                    std::uintptr_t sectionBaseAddress = s.VirtualAddress;
                    std::uintptr_t sectionEndAddress =
                            sectionBaseAddress + s.Misc.VirtualSize;

                    auto callback_data = static_cast<CallbackData *>(N);
                    if (callback_data->address >= sectionBaseAddress &&
                        callback_data->address < sectionEndAddress) {
                        callback_data->result =
                                s.PointerToRawData +
                                (callback_data->address - sectionBaseAddress);

                        callback_data->found = true;
                        return 1;
                    }

                    return 0;
                };

                CallbackData callback_data = {false, address, 0U};
                IterSec(pe, L_inspectSection, &callback_data);

                if (!callback_data.found) {
                    return false;
                }

                result = callback_data.result;
                return true;

            } else if (destination_type == AddressType::VirtualAddress) {
                result = image_base_address + address;
                return true;
            }

            return false;
        }

        case AddressType::VirtualAddress: {
            if (address < image_base_address) {
                return false;
            }

            std::uint64_t rva = address - image_base_address;
            return convertAddress(pe,
                                  rva,
                                  AddressType::RelativeVirtualAddress,
                                  destination_type,
                                  result);
        }

        default: {
            return false;
        }
    }
}


int printExps(void *N,
              const peparse::VA &funcAddr,
              const std::string &mod,
              const std::string &func) {
    static_cast<void>(N);

    auto address = static_cast<std::uint32_t>(funcAddr);

    std::cout << "EXP: ";
    std::cout << mod;
    std::cout << "!";
    std::cout << func;
    std::cout << ": 0x";
    std::cout << std::hex << address;
    std::cout << "\n";
    return 0;
}

int printImports(void *N,
                 const peparse::VA &impAddr,
                 const std::string &modName,
                 const std::string &symName) {
    static_cast<void>(N);

    auto address = static_cast<std::uint32_t>(impAddr);

    std::cout << "0x" << std::hex << address << " " << modName << "!" << symName;
    std::cout << "\n";
    return 0;
}

int printSymbols(void *N,
                 const std::string &strName,
                 const uint32_t &value,
                 const int16_t &sectionNumber,
                 const uint16_t &type,
                 const uint8_t &storageClass,
                 const uint8_t &numberOfAuxSymbols) {
    static_cast<void>(N);

    std::cout << "Symbol Name: " << strName << "\n";
    std::cout << "Symbol Value: 0x" << std::hex << value << "\n";

    std::cout << "Symbol Section Number: ";
    switch (sectionNumber) {
        case IMAGE_SYM_UNDEFINED:
            std::cout << "UNDEFINED";
            break;
        case IMAGE_SYM_ABSOLUTE:
            std::cout << "ABSOLUTE";
            break;
        case IMAGE_SYM_DEBUG:
            std::cout << "DEBUG";
            break;
        default:
            std::cout << sectionNumber;
            break;
    }
    std::cout << "\n";

    std::cout << "Symbol Type: ";
    switch (type) {
        case IMAGE_SYM_TYPE_NULL:
            std::cout << "NULL";
            break;
        case IMAGE_SYM_TYPE_VOID:
            std::cout << "VOID";
            break;
        case IMAGE_SYM_TYPE_CHAR:
            std::cout << "CHAR";
            break;
        case IMAGE_SYM_TYPE_SHORT:
            std::cout << "SHORT";
            break;
        case IMAGE_SYM_TYPE_INT:
            std::cout << "INT";
            break;
        case IMAGE_SYM_TYPE_LONG:
            std::cout << "LONG";
            break;
        case IMAGE_SYM_TYPE_FLOAT:
            std::cout << "FLOAT";
            break;
        case IMAGE_SYM_TYPE_DOUBLE:
            std::cout << "DOUBLE";
            break;
        case IMAGE_SYM_TYPE_STRUCT:
            std::cout << "STRUCT";
            break;
        case IMAGE_SYM_TYPE_UNION:
            std::cout << "UNION";
            break;
        case IMAGE_SYM_TYPE_ENUM:
            std::cout << "ENUM";
            break;
        case IMAGE_SYM_TYPE_MOE:
            std::cout << "IMAGE_SYM_TYPE_MOE";
            break;
        case peparse::IMAGE_SYM_TYPE_BYTE:
            std::cout << "BYTE";
            break;
        case peparse::IMAGE_SYM_TYPE_WORD:
            std::cout << "WORD";
            break;
        case IMAGE_SYM_TYPE_UINT:
            std::cout << "UINT";
            break;
        case peparse::IMAGE_SYM_TYPE_DWORD:
            std::cout << "DWORD";
            break;
        default:
            std::cout << "UNKNOWN";
            break;
    }
    std::cout << "\n";

    std::cout << "Symbol Storage Class: ";
    switch (storageClass) {
        case IMAGE_SYM_CLASS_END_OF_FUNCTION:
            std::cout << "FUNCTION";
            break;
        case IMAGE_SYM_CLASS_NULL:
            std::cout << "NULL";
            break;
        case IMAGE_SYM_CLASS_AUTOMATIC:
            std::cout << "AUTOMATIC";
            break;
        case IMAGE_SYM_CLASS_EXTERNAL:
            std::cout << "EXTERNAL";
            break;
        case IMAGE_SYM_CLASS_STATIC:
            std::cout << "STATIC";
            break;
        case IMAGE_SYM_CLASS_REGISTER:
            std::cout << "REGISTER";
            break;
        case IMAGE_SYM_CLASS_EXTERNAL_DEF:
            std::cout << "EXTERNAL DEF";
            break;
        case IMAGE_SYM_CLASS_LABEL:
            std::cout << "LABEL";
            break;
        case IMAGE_SYM_CLASS_UNDEFINED_LABEL:
            std::cout << "UNDEFINED LABEL";
            break;
        case IMAGE_SYM_CLASS_MEMBER_OF_STRUCT:
            std::cout << "MEMBER OF STRUCT";
            break;
        default:
            std::cout << "UNKNOWN";
            break;
    }
    std::cout << "\n";

    std::cout << "Symbol Number of Aux Symbols: "
              << static_cast<std::uint32_t>(numberOfAuxSymbols) << "\n";

    return 0;
}


int printRsrc(void *N, const peparse::resource &r) {
    static_cast<void>(N);

    if (r.type_str.length())
        std::cout << "Type (string): " << r.type_str << "\n";
    else
        std::cout << "Type: 0x" << std::hex << r.type << "\n";

    if (r.name_str.length())
        std::cout << "Name (string): " << r.name_str << "\n";
    else
        std::cout << "Name: 0x" << std::hex << r.name << "\n";

    if (r.lang_str.length())
        std::cout << "Lang (string): " << r.lang_str << "\n";
    else
        std::cout << "Lang: 0x" << std::hex << r.lang << "\n";

    std::cout << "Codepage: 0x" << std::hex << r.codepage << "\n";
    std::cout << "RVA: " << std::dec << r.RVA << "\n";
    std::cout << "Size: " << std::dec << r.size << "\n";
    return 0;
}

int printSecs(void *N,
              const peparse::VA &secBase,
              const std::string &secName,
              const peparse::image_section_header &s,
              const peparse::bounded_buffer *data) {
    static_cast<void>(N);
    static_cast<void>(s);

    std::cout << "Sec Name: " << secName << "\n";
    std::cout << "Sec Base: 0x" << std::hex << secBase << "\n";
    if (data)
        std::cout << "Sec Size: " << std::dec << data->bufLen << "\n";
    else
        std::cout << "Sec Size: 0"
                  << "\n";
    return 0;
}
