/*
Master System memory map:
0x0000-0x3FFF : Rom Slot 1
0x4000-0x7FFF : Rom Slot 2
0x8000-0xBFFF : Rom Slot 3 / RAM Slot
0xC000-0xDFFF : RAM
0xE000-0xFFFF : Mirrored RAM
 */
#include <iostream>
#include <fstream>
#include "Cartridge.h"
#include "Memory.h"
#include "Utils.h"
#include "Exceptions.h"
#include <regex>
#include <sys/stat.h>

Memory::Memory(Cartridge *cart, Config *config) {
    smsCartridge = cart;
    this->config = config;
    controlRegister = 0xBF; // Enable cartridge slot by default for now // TODO enable BIOS if one is detected

    // Clear memory
    for (unsigned char & i : ram) {
        i = 0x0;
    }

    // Clear RAM banks
    for (auto &bank: ramBank) {
        for (unsigned char &i: bank) {
            i = 0x0;
        }
    }

    for (unsigned short &memoryPage: memoryPages) {
        memoryPage = 0x0;
    }
    memoryPages[1] = 1;
    memoryPages[2] = 2;

    currentPage3RamBank = -1;
    CRAMChangedSinceLastWrite = false;
    framesSinceLastCRAMWrite = 0;
}

Memory::~Memory() = default;

void Memory::init() {
    readCRAM();
}

/**
 * [read Read from system memory]
 * @param  location [Memory location to read from]
 * @return          [Value within memory at location]
 */
unsigned char Memory::read(unsigned short location) {
    // Handle mirroring
    if (location >= 0xFFFC) {
        location -= 0x2000;
    }

    if (!smsCartridge->isCodemasters() && location < 0x400) {
        return readMedia(location);
    }

    if (location < 0x4000) {
        // Page 1
        unsigned long address = location + (0x4000 * memoryPages[0]);
        return readMedia(address);
    }

    if (location < 0x8000) {
        // Page 2
        unsigned long address = location + (0x4000 * memoryPages[1]);
        address -= 0x4000; // Remove offset
        return readMedia(address);
    }

    if (location < 0xC000) {
        // Page 3 (or extra RAM)
        if (currentPage3RamBank >= 0) {
            return ramBank[currentPage3RamBank][location - 0x8000];
        }

        unsigned long address = location + (0x4000 * memoryPages[2]);
        address -= 0x8000;
        return readMedia(address);
    }

    return ram[location];
}

unsigned char Memory::readMedia(unsigned long location) {
    // TODO allow reading from BIOS, also determine which priority these should be if multiple flags are turned on
    if (!Utils::testBit(MemoryControlRegisterFlags::enableCartridgeSlot, controlRegister)) {
        return smsCartridge->read(location);
    }

    return 0xFF;
}

unsigned short Memory::read16Bit(unsigned short location) {
    return read(location) + (read(location+1) << 8);
}

void Memory::write(unsigned short location, unsigned short value) {
    write(location, (unsigned char)(value & 0x00FF));
    write(location+1, (unsigned char)(value >> 8));
}

/**
 * [write Writes to system memory]
 * @param location [Memory location to write to]
 * @param value    [The byte to write]
 */
void Memory::write(unsigned short location, unsigned char value) {

    handleMemoryPaging(location, value);

    if (location < 0x8000) {
        // Attempting to write to ROM, disallow this...
        return;
    }

    if (location < 0xC000) {
        // Allow a write if RAM is banked into this address range
        // TODO should the media/memory control register effect this also for when attempting to write to CART RAM?
        if (currentPage3RamBank < 0) {
            return;
        }

        ramBank[currentPage3RamBank][location - 0x8000] = value;
        CRAMChangedSinceLastWrite = true;
        framesSinceLastCRAMWrite = 0;
        return;
    }

    // If we've reached this point, it's all good to write to RAM
    ram[location] = value;

    // Handle mirrored addresses
    if (location >= 0xC000 && location < 0xDFFC) {
        ram[location - 0x2000] = value;
        return;
    }

    if (location >= 0xE000) {
        ram[location - 0x2000] = value;
        return;
    }

}

/**
 * [Memory::memoryPage Perform a memory page operation]
 * @param Codemasters [Whether the game is codemasters or not]
 */
void Memory::handleMemoryPaging(unsigned short location, unsigned char value) {

    if (!smsCartridge->isCodemasters()) {

        if (location < 0xFFFC) {
            return;
        }

        unsigned short page = value & smsCartridge->getBankMask();

        switch (location) {
            case 0xFFFC:
                if (Utils::testBit(3, value)) {
                    if (Utils::testBit(2, value)) {
                        currentPage3RamBank = 1;
                    } else {
                        currentPage3RamBank = 0;
                    }
                } else {
                    currentPage3RamBank = -1;
                }
                break;
            case 0xFFFD:
                memoryPages[0] = page;
                break;
            case 0xFFFE:
                memoryPages[1] = page;
                break;
            case 0xFFFF:
                // ROM banking in slot 3 - Only allow if there is no RAM there
                if (!Utils::testBit(3, ram[0xFFFC])) {
                    memoryPages[2] = page;
                }
                break;
            default:
                break;
        }
        return;
    }

    if (location == 0x0 || location == 0x4000 || location == 0x8000) {
        unsigned short page = value & smsCartridge->getBankMask();

        switch (location) {
            case 0x0:
                memoryPages[0] = page;
                break;
            case 0x4000:
                memoryPages[1] = page;
                break;
            case 0x8000:
                memoryPages[2] = page;
                break;
            default:
                break;
        }
    }
}

void Memory::writeMediaControlRegister(unsigned char value) {
    controlRegister = value;
}

void Memory::handleCRAMWriting() {
    /* Automatically writes the CRAM to a file if the CRAM has changed and has not been written after a certain number of frames
     * Should be called for every emulated frame */

    if (!config->getAutoSaveCRAM()) {
        return;
    }

    if (!CRAMChangedSinceLastWrite) {
        return;
    }

    if ((++framesSinceLastCRAMWrite) < 120) {
        return;
    }

    writeCRAM();
}

void Memory::writeCRAM() {

    if (!config->getPersistCRAM()) {
        return;
    }

    CRAMChangedSinceLastWrite = false;
    framesSinceLastCRAMWrite = 0;

    std::string fileName = getCRAMSaveFilePath();

    if (fileName.empty()) {
        throw IOException("Unable to get file path to save CRAM to");
    }

    std::ofstream fileOut(fileName, std::ios::out | std::ios::binary);

    if (!fileOut) {
        throw IOException(Utils::implodeString({"Unable to save CRAM to file, unable to write to '", fileName, "'"}));
    }

    // Write the header to the file
    std::vector<unsigned char> headerVector = getCRAMDumpFileHeader();

    unsigned char header[headerVector.size()];
    std::copy(headerVector.begin(), headerVector.end(), header);

    fileOut.write((char*)&header, (long)sizeof(header));

    for (auto &CRAM : ramBank) {
        fileOut.write((char*)CRAM, sizeof(CRAM));
    }

    if (!fileOut.good()) {
        throw IOException(Utils::implodeString({"Unable to save CRAM to file, unable to write to '", fileName, "'"}));
    }

    fileOut.close();
}

void Memory::readCRAM() {

    if (!config->getPersistCRAM()) {
        return;
    }

    std::string fileName = getCRAMSaveFilePath();

    if (fileName.empty()) {
        return;
    }

    struct stat fileStat{};
    int fileSize;

    int fileStatus = stat(fileName.c_str(), &fileStat);

    if (fileStatus != 0) {
        // File does not exist
        return;
    }

    fileSize = (int)fileStat.st_size;

    std::vector<unsigned char> expectedHeader = getCRAMDumpFileHeader();
    int expectedHeaderSize = (int)expectedHeader.size();

    int expectedFileSize = (expectedHeaderSize + (0x4000 * 2));

    if (fileSize != expectedFileSize) {
        // Fle too large - abort
        std::cout << "Error - CRAM save file is the incorrect size (" << fileSize << ") bytes (Expected " << expectedFileSize << ")" << std::endl;
        return;
    }

    typedef std::istream_iterator<unsigned char> istream_iterator;

    // Read the file into temporary storage
    std::ifstream fileStream(fileName, std::ios::binary);
    std::vector<unsigned char> tempStorage;

    fileStream >> std::noskipws; // Do not skip white space, we want every single character of the file.
    std::copy(istream_iterator(fileStream), istream_iterator(),
              std::back_inserter(tempStorage)); // Copy the contents of the file into the temporary storage vector

    // Ensure the header matches what we are looking for
    for (int i = 0; i < (int)expectedHeader.size(); i++) {
        if (expectedHeader[i] != tempStorage[i]) {
            std::cout<<"Error loading CRAM save file - header does not match expected"<<std::endl;
            return;
        }
    }

    // Copy the temporary data into the emulated CRAM
    for (int i = 0; i < 0x4000; i++) {
        ramBank[0][i] = tempStorage[expectedHeaderSize + i];
    }

    for (int i = 0; i < 0x4000; i++) {
        ramBank[1][i] = tempStorage[expectedHeaderSize + 0x4000 + i];
    }

}

std::vector<unsigned char> Memory::getCRAMDumpFileHeader() {
    // This header will be used to identify a CRAM dump created by this emulator
    return {
        0x4D, 0x41, 0x53, 0x54,
        0x45, 0x52, 0x4E, 0x4F,
        0x53, 0x54, 0x41, 0x4C,
        0x47, 0x49, 0x41, 0x20,
        0x50, 0x52, 0x41, 0x4D,
        0x20, 0x44, 0x55, 0x4D,
        0x50, 0x31, 0x00, 0x00
    };
}

void Memory::shutdown() {
    // Writes CRAM to a file, should be called when the program exits.
    if (!doesCRAMContainData()) {
        return;
    }

    writeCRAM();
}

bool Memory::doesCRAMContainData() {
    // Returns true if cram contains any data
    for (auto & i : ramBank) {
        for (auto &datum : i) {
            if (datum != 0x0) {
                return true;
            }
        }
    }

    return false;
}

std::string Memory::getCRAMSaveFilePath() {
    // Save the file into the directory given in the config file if there is one, otherwise save it in the same location as the game ROM
    std::string directoryPath = !config->getCRAMSaveLocation().empty() ? config->getCRAMSaveLocation() : smsCartridge->getROMFilePath();

    if (directoryPath[directoryPath.length() -1] != '/') {
        directoryPath.append("/");
    }

    std::string romFileName = smsCartridge->getROMFileName();

    if (romFileName.empty()) {
        return "";
    }

    directoryPath.append(romFileName);
    directoryPath.append(".mncram");

    return directoryPath;
}

MemorySaveStateData* Memory::getSaveStateData() {

    // TODO should I also store CRAM as part of the save state? determine later.
    MemorySaveStateData* result = new MemorySaveStateData();

    for (int i = 0; i < 3; i++) {
        result->memoryPages[i] = memoryPages[i];
    }

    result->controlRegister = controlRegister;

    for (int i = 0; i < 0x10000; i++) {
        result->ram[i] = ram[i];
    }

    return result;
}

void Memory::restoreState(MemorySaveStateData *data) {

    for (int i = 0; i < 0x10000; i++) {
        ram[i] = data->ram[i];
    }

    for (int i = 0; i < 3; i++) {
        memoryPages[i] = data->memoryPages[i];
    }

    controlRegister = data->controlRegister;
}