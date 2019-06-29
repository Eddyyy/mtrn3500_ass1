
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
    if (channel <= 0x05 && value <= 0x0FFF) {
        //write to dac registers based on channel
        //and ensure that the conversion is executed
        this->eops->outb((uint8_t)(value & 0x00FF), this->baseAddr+channel*2);
        this->eops->outb((uint8_t)((value & 0x0F00)>>8), this->baseAddr+channel*2+1);
        this->eops->inb(this->baseAddr+channel*2);
        this->eops->inb(this->baseAddr+channel*2+1);
    }
}

//Similar to the arduino map function which scales an input voltage
//from a input range and offset to 0-1 then scales it to an output range and offset
double EmbeddedDevice::DAC06::mapVal(double input, double minSrc, double maxSrc, double minDest, double maxDest) {
    return ((input-minSrc)/(maxSrc-minSrc))*(maxDest-minDest)+minDest;
}

void EmbeddedDevice::DAC06::analogOutputVoltage(uint8_t channel, double desired_voltage) {
    //scale desired_voltage and output raw value
    uint16_t rawValue = (uint16_t)this->mapVal(desired_voltage, MIN_DAC_V, MAX_DAC_V, MIN_12BIT, MAX_12BIT);
    this->analogOutputRaw(channel, rawValue);
}
