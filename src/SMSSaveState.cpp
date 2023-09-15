#include "SMSSaveState.h"
#include "sys/stat.h"
#include <iostream>
#include <fstream>

class DataOutput {
public:

    DataOutput() = default;

    void write(unsigned char value) {
        data.push_back(value);
    }

    void write(unsigned short value) {
        unsigned char hi = value >> 8;
        unsigned char lo = value & 0xFF;
        write(hi);
        write(lo);
    }

    void write(bool value) {
        unsigned char valueToWrite = value ? 1 : 0;
        write(valueToWrite);
    }

    void write(std::vector<unsigned char> value) {
        for (auto &datum : value) {
            write(datum);
        }
    }

    std::vector<unsigned char> getData() {
        return data;
    }

private:
    std::vector<unsigned char> data;
};

class DataInput {
public:

    DataInput(std::vector<unsigned char> data) {
        this->data = data;
        location = 0;
    }

    unsigned char read8Bit() {

        if (location >= data.size()) {
            throw IOException(Utils::implodeString({"Tried to read across the end of a save state file (", std::to_string(location), " of ", std::to_string(data.size()), ")"}));
        }

        return data[location++];
    }

    unsigned short read16Bit() {
        return (read8Bit() << 8) + read8Bit();
    };

    bool readBoolean() {
        return read8Bit() != 0;
    }

private:
    std::vector<unsigned char> data;
    unsigned long location;
};

SMSSaveState::SMSSaveState(unsigned char id,
                           MemorySaveStateData *memoryData,
                           Z80SaveStateData *z80SaveStateData,
                           VDPSaveStateData *vdpSaveStateData,
                           PSGSaveStateData *psgSaveStateData,
                           Config *config,
                           Cartridge *cartridge) {

    // Store required data
    this->id = id;
    this->memoryData = memoryData;
    this->z80SaveStateData = z80SaveStateData;
    this->vdpSaveStateData = vdpSaveStateData;
    this->psgSaveStateData = psgSaveStateData;

    // Determine the file name to save the data to
    std::string fileName = getFileName(config, cartridge);

    if (fileName.empty()) {
        throw IOException(Utils::implodeString({"Unable to create file name for save state ", std::to_string(id)}));
    }

    // Save the save state into a file
    writeToFile(fileName);

    isLoaded = true;
}

SMSSaveState::SMSSaveState(unsigned char id,
                           Config *config,
                           Cartridge *cartridge) {
    this->id = id;
    isLoaded = loadFromFile(getFileName(config, cartridge));
}

SMSSaveState::~SMSSaveState() {
    delete(memoryData);
    delete(z80SaveStateData);
    delete(vdpSaveStateData);
    delete(psgSaveStateData);
}

MemorySaveStateData *SMSSaveState::getMemoryData() {
    return memoryData;
}

Z80SaveStateData *SMSSaveState::getZ80SaveStateData() {
    return z80SaveStateData;
}

VDPSaveStateData *SMSSaveState::getVdpSaveStateData() {
    return vdpSaveStateData;
}

PSGSaveStateData *SMSSaveState::getPsgSaveStateData() {
    return psgSaveStateData;
}

bool SMSSaveState::getIsLoaded() {
    return isLoaded;
}

std::string SMSSaveState::getFileName(Config *config, Cartridge *cartridge) {
    std::string path = !config->getSaveStateLocation().empty() ? config->getSaveStateLocation() : cartridge->getROMFilePath();

    if (path[path.length() -1] != '/') {
        path.append("/");
    }

    std::string romFileName = cartridge->getROMFileName();

    if (romFileName.empty()) {
        return "";
    }

    path.append(romFileName);
    path.append(".massstate");
    path.append(std::to_string(id));

    return path;
}

void SMSSaveState::writeToFile(const std::string& fileName) {

    // Actual data positioning in the file does not matter, just make sure you get everything in the right order.
    DataOutput *output = new DataOutput();

    std::vector <unsigned char> header = getFileHeader();

    for (auto &datum : header) {
        output->write(datum);
    }

    // Build Memory data
    for (unsigned char datum : memoryData->ram) {
        output->write(datum);
    }

    for (unsigned short datum : memoryData->memoryPages) {
        output->write(datum);
    }

    output->write(memoryData->controlRegister);

    // Build Z80 data
    for (unsigned short &registerValue : z80SaveStateData->registers) {
        output->write(registerValue);
    }

    output->write(z80SaveStateData->registerI);
    output->write(z80SaveStateData->registerR);
    output->write(z80SaveStateData->programCounter);
    output->write(z80SaveStateData->stackPointer);
    output->write(z80SaveStateData->iff1);
    output->write(z80SaveStateData->iff2);
    output->write(z80SaveStateData->enableInterrupts);

    // Build VDP data
    for (unsigned char &datum : vdpSaveStateData->vRAM) {
        output->write(datum);
    }

    for (unsigned char &datum : vdpSaveStateData->cRAM) {
        output->write(datum);
    }

    output->write(vdpSaveStateData->statusRegister);

    for (unsigned char &datum : vdpSaveStateData->registers) {
        output->write(datum);
    }

    output->write(vdpSaveStateData->controlWord);
    output->write(vdpSaveStateData->readBuffer);
    output->write(vdpSaveStateData->requestInterrupt);
    output->write(vdpSaveStateData->isSecondControlWrite);
    output->write(vdpSaveStateData->vScroll);
    output->write(vdpSaveStateData->vCounter);
    output->write(vdpSaveStateData->lineInterruptCounter);
    output->write(vdpSaveStateData->vCounterJumpCount);

    // Build PSG data
    output->write(psgSaveStateData->selectedRegister);
    output->write(psgSaveStateData->hasSelectedVolumeRegister);

    for (unsigned short &datum : psgSaveStateData->channelFrequencies) {
        output->write(datum);
    }

    for (unsigned char &datum : psgSaveStateData->channelVolumes) {
        output->write(datum);
    }

    std::vector<unsigned char> outputVector = output->getData();

    // TODO output to a file
    unsigned char outputData[outputVector.size()];
    std::copy(outputVector.begin(), outputVector.end(), outputData);

    std::ofstream fileOut(fileName, std::ios::out | std::ios::binary);

    if (!fileOut) {
        throw IOException(Utils::implodeString({"Unable to save CRAM to file, unable to write to '", fileName, "'"}));
    }

    fileOut.write((char *)&outputData, (long)sizeof(outputData));

    if (!fileOut.good()) {
        throw IOException(Utils::implodeString({"Unable to save CRAM to file, unable to write to '", fileName, "'"}));
    }

    fileOut.close();
}

bool SMSSaveState::loadFromFile(const std::string &fileName) {

    // TODO create a method for loading save states from previous versions of the emulator if the format ever changes
    if (fileName.empty()) {
        return false;
    }

    struct stat fileStat{};

    int fileSize;

    int fileStatus = stat(fileName.c_str(), &fileStat);

    if (fileStatus != 0) {
        // File does not exist
        return false;
    }

    fileSize = (int)fileStat.st_size;
    int expectedFileSize = 0x140A1;

    if (fileSize != expectedFileSize) {
        // Fle too large - abort
        std::cout << "Error - Save state " << id << " is the incorrect size (" << fileSize << ") bytes (Expected " << expectedFileSize << ")" << std::endl;
        return false;
    }

    typedef std::istream_iterator<unsigned char> istream_iterator;

    std::ifstream fileStream(fileName, std::ios::binary);
    std::vector<unsigned char> tempStorage;

    fileStream >> std::noskipws; // Do not skip white space, we want every single character of the file.
    std::copy(istream_iterator(fileStream), istream_iterator(),
              std::back_inserter(tempStorage)); // Copy the contents of the file into the temporary storage vector

    std::vector<unsigned char> expectedHeader = getFileHeader();
    int expectedHeaderSize = (int)expectedHeader.size();

    /* When reading data, we don't care about the actual positions in the file because of the DataInput class, just make
     * sure that you read things in the same order as they were saved. */

    auto *input = new DataInput(tempStorage);

    for (int i = 0; i < expectedHeaderSize; i++) {
        if (expectedHeader[i] != input->read8Bit()) {
            std::cout<<"Failed to read save state " << id << " (Invalid header)"<<std::endl;
            return false;
        }
    }

    // Create objects
    memoryData = new MemorySaveStateData();
    z80SaveStateData = new Z80SaveStateData();
    vdpSaveStateData = new VDPSaveStateData();
    psgSaveStateData = new PSGSaveStateData();

    // Memory
    for (unsigned char & datum : memoryData->ram) {
        datum = input->read8Bit();
    }

    for (unsigned short & datum : memoryData->memoryPages) {
        datum = input->read16Bit();
    }

    memoryData->controlRegister = input->read8Bit();

    // Z80
    for (unsigned short &registerValue : z80SaveStateData->registers) {
        registerValue = input->read16Bit();
    }

    z80SaveStateData->registerI = input->read8Bit();
    z80SaveStateData->registerR = input->read8Bit();
    z80SaveStateData->programCounter = input->read16Bit();
    z80SaveStateData->stackPointer = input->read16Bit();
    z80SaveStateData->iff1 = input->readBoolean();
    z80SaveStateData->iff2 = input->readBoolean();
    z80SaveStateData->enableInterrupts = input->readBoolean();

    // VDP
    for (unsigned char &datum : vdpSaveStateData->vRAM) {
        datum = input->read8Bit();
    }

    for (unsigned char &datum : vdpSaveStateData->cRAM) {
        datum = input->read8Bit();
    }

    vdpSaveStateData->statusRegister = input->read8Bit();

    for (unsigned char &datum : vdpSaveStateData->registers) {
        datum = input->read8Bit();
    }

    vdpSaveStateData->controlWord = input->read16Bit();
    vdpSaveStateData->readBuffer = input->read8Bit();
    vdpSaveStateData->requestInterrupt = input->readBoolean();
    vdpSaveStateData->isSecondControlWrite = input->readBoolean();
    vdpSaveStateData->vScroll = input->read8Bit();
    vdpSaveStateData->vCounter = input->read8Bit();
    vdpSaveStateData->lineInterruptCounter = input->read8Bit();
    vdpSaveStateData->vCounterJumpCount = input->read8Bit();

    // PSG
    psgSaveStateData->selectedRegister = input->read8Bit();
    psgSaveStateData->hasSelectedVolumeRegister = input->readBoolean();

    for (unsigned short &datum : psgSaveStateData->channelFrequencies) {
        datum = input->read16Bit();
    }

    for (unsigned char &datum : psgSaveStateData->channelVolumes) {
        datum = input->read8Bit();
    }

    delete(input);

    return true;
}

std::vector<unsigned char> SMSSaveState::getFileHeader() {
    return {
        0x4D, 0x41, 0x53, 0x54,
        0x41, 0x4C, 0x47, 0x49,
        0x41, 0x20, 0x53, 0x41,
        0x56, 0x45, 0x53, 0x54,
        0x41, 0x54, 0x45, 0x31,
        0x00, 0x00, 0x00, 0x00
    };
}