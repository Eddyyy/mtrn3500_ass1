
#include "DAC06.h"

#define MIN_DAC_V -5
#define MAX_DAC_V 5
#define MIN_12BIT 0
#define MAX_12BIT 4095

EmbeddedDevice::DAC06::DAC06(EmbeddedOperations *eops, uint32_t base_addr) {
    //Initisalise base vals
    this->eops = eops;
    this->baseAddr = base_addr;

    //check perms
    if (this->eops->ioperm(this->baseAddr, 16, 1) != 0) {
        std::cout << "fail perm" << std::endl;
    }
}

void EmbeddedDevice::DAC06::analogOutputRaw(uint8_t channel, uint16_t value) {
    if (channel <= 5 && channel >= 0 && value <= 0x0FFF) {
        this->eops->outb((uint8_t)(value & 0x00FF), this->baseAddr+value*2);
        this->eops->outb((uint8_t)(value & 0x0F00), this->baseAddr+value*2+1);
        this->eops->inb(this->baseAddr+value*2);
        this->eops->inb(this->baseAddr+value*2+1);
    }
}

double EmbeddedDevice::DAC06::mapVal(double input, double minSrc, double maxSrc, double minDest, double maxDest) {
    return ((input-minSrc)/(maxSrc-minSrc))*(maxDest-minDest)+minDest;
}

void EmbeddedDevice::DAC06::analogOutputVoltage(uint8_t channel, double desired_voltage) {
    uint16_t rawValue = (uint16_t)this->mapVal(desired_voltage, MIN_12BIT, MAX_12BIT, MIN_DAC_V, MAX_DAC_V);
    this->analogOutputRaw(channel, rawValue);
}
