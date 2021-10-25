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