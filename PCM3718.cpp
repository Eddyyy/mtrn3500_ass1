
#include "PCM3718.h"

#define DIGI_LO_PORT 0x03
#define DIGI_HI_PORT 0x0B
#define AD_RANGE_PORT 0x01

using namespace std;

PCM3718(EmbeddedOperations *eops, uint32_t base_addr) {
    this->eops = eops;
    this->baseAddr = base_addr;
    this->analogRange = 0x00;
    if (this->eops->ioperm(this->baseAddr, 16, 1) != 0) {
        std::cout << "fail perm" << std::endl;
    }
}
PCM3718(EmbeddedOperations *eops, uint32_t base_addr, uint8_t analog_range) {
    this->eops = eops;
    this->baseAddr = base_addr;
    this->analogRange = (analog_range <= 0x0B) ? analogRange : 0;
}
~PCM3718() {
    delete this->eops;
}

uint16_t digitalInput() {
    uint8_t loIn = this->eops->inb(this->baseAddr+DIGI_LO_PORT);
    uint8_t hiIn = this->eops->inb(this->baseAddr+DIGI_HI_PORT);
    return (uint16_t)loIn | ((uint16_t)hiIn)<<8;
}

uint8_t digitalByteInput(bool high_byte) {
    uint32_t inPort;
    if (high_byte) {
        inPort = this->baseAddr+DIGI_HI_PORT;
    } else {
        inPort = this->baseAddr+DIGI_LO_PORT;
    }
    return this->eops->inb(inPort);
}

bool digitalBitInput(uint8_t bit) {
    if (bit > 0x0F) {
        std::cout << "Incorrect Bit range" << std::endl;
        return 1;
    }
    uint16_t bytIn =  this->digitalInput();
    return (bool)(bytIn & 1<<bit);
}

void digitalOutput(uint16_t value) {
    this->eops->outb((uint8_t)(value & 0x0F), this->baseAddr+DIGI_LO_PORT);
    this->eops->outb((uint8_t)((value & 0xF0)>>8), this->baseAddr+DIGI_HI_PORT);
}

void digitalByteOutput(bool high_byte, uint8_t value) {
    uint32_t outPort;
    if (high_byte) {
        outPort = this->baseAddr+DIGI_HI_PORT;
    } else {
        outPort = this->baseAddr+DIGI_LO_PORT;
    }
    this->eops->outb(value, outPort);
}

void setRange(uint8_t new_analog_range) {
    this->analogRange = (new_analog_range <= 0x0B) ? new_analog_range : this->analogRange;
}

double analogInput(uint8_t channel) const {
    this->eops->outb(this->analogRange, this->baseAddr+AD_RANGE_PORT);
    uint8_t channelIn = (channel <= 0x0F) ? (channel | channel<<4) : 0;
    this->eops->outb(channelIn)
}

