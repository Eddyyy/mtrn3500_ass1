
#include "PCM3718.h"

#define AD_LO_CH_PORT 0x00
#define AD_HI_RANGE_PORT 0x01
#define AD_CHSEL_PORT 0x02
#define DIGI_LO_PORT 0x03
#define AD_STATUS_PORT 0x08
#define DIGI_HI_PORT 0x0B

#define MIN_12BIT 0
#define MAX_12BIT 4095

using namespace std;

EmbeddedDevice::PCM3718::PCM3718(EmbeddedOperations *eops, uint32_t base_addr) {
    //Initisalise base vals
    this->eops = eops;
    this->baseAddr = base_addr;
    this->analogRange = 0x00;

    //check perms
    if (this->eops->ioperm(this->baseAddr, 16, 1) != 0) {
        std::cout << "fail perm" << std::endl;
    }
}

EmbeddedDevice::PCM3718::PCM3718(EmbeddedOperations *eops, uint32_t base_addr, uint8_t analog_range) {
    //Initisalise base vals
    this->eops = eops;
    this->baseAddr = base_addr;
    this->analogRange = (analog_range <= 0x0B) ? analogRange : 0;
    
    //check perms
    if (this->eops->ioperm(this->baseAddr, 16, 1) != 0) {
        std::cout << "fail perm" << std::endl;
    }
}

/*
EmbeddedDevice::PCM3718::~PCM3718() {
    delete this->eops;
}
*/

uint16_t EmbeddedDevice::PCM3718::digitalInput() {
    //Read an combine input from both bytes
    uint8_t loIn = this->eops->inb(this->baseAddr+DIGI_LO_PORT);
    uint8_t hiIn = this->eops->inb(this->baseAddr+DIGI_HI_PORT);
    return (uint16_t)loIn | ((uint16_t)hiIn)<<8;
}

uint8_t EmbeddedDevice::PCM3718::digitalByteInput(bool high_byte) {
    uint32_t inPort;
    //check which byte to read
    if (high_byte) {
        inPort = this->baseAddr+DIGI_HI_PORT;
    } else {
        inPort = this->baseAddr+DIGI_LO_PORT;
    }

    //Read byte
    return this->eops->inb(inPort);
}

bool EmbeddedDevice::PCM3718::digitalBitInput(uint8_t bit) {
    //Check if bit is in range
    if (bit > 0x0F) {
        std::cout << "Incorrect Bit range" << std::endl;
        return 1;
    }

    //read all bytes and check specific bit
    uint16_t bytIn =  this->digitalInput();
    return (bool)(bytIn & 1<<bit);
}

void EmbeddedDevice::PCM3718::digitalOutput(uint16_t value) {
    //Split value into hi and lo and ouput to respective ports
    this->eops->outb((uint8_t)(value & 0x00FF), this->baseAddr+DIGI_LO_PORT);
    this->eops->outb((uint8_t)((value & 0xFF00)>>8), this->baseAddr+DIGI_HI_PORT);
}

void EmbeddedDevice::PCM3718::digitalByteOutput(bool high_byte, uint8_t value) {
    //Check which port to output to
    uint32_t outPort;
    if (high_byte) {
        outPort = this->baseAddr+DIGI_HI_PORT;
    } else {
        outPort = this->baseAddr+DIGI_LO_PORT;
    }
    //Output byte
    this->eops->outb(value, outPort);
}

void EmbeddedDevice::PCM3718::setRange(uint8_t new_analog_range) {
    //Store new range
    this->analogRange = (new_analog_range <= 0x0B) ? new_analog_range : this->analogRange;
}

//Map a value within src range to a proportional spot on the dest range
double EmbeddedDevice::PCM3718::mapVal(double input, double minSrc, double maxSrc, double minDest, double maxDest) const {
    return ((input-minSrc)/(maxSrc-minSrc))*(maxDest-minDest)+minDest;
}

double EmbeddedDevice::PCM3718::analogInput(uint8_t channel) const {

    //Clear Status bit
    this->eops->outb(0<<4, this->baseAddr+AD_STATUS_PORT);

    //Select Channel
    uint8_t channelIn = (channel <= 0x0F) ? (channel | channel<<4) : 0;
    this->eops->outb(channelIn, this->baseAddr+AD_CHSEL_PORT);

    //Select Range
    this->eops->outb(this->analogRange, this->baseAddr+AD_HI_RANGE_PORT);

    //Trigger Conversion
    this->eops->outb(0xFF, this->baseAddr+AD_LO_CH_PORT);

    //Wait For conversion
    while (!(this->eops->inb(this->baseAddr+AD_STATUS_PORT) & 1<<4)) {
        usleep(100);
    }

    //Read analog
    uint8_t hexInLo = this->eops->inb(this->baseAddr+AD_LO_CH_PORT);
    uint8_t hexInHi = this->eops->inb(this->baseAddr+AD_HI_RANGE_PORT);
    uint16_t hexInByte = (uint16_t)(hexInLo>>4) + (((uint16_t)hexInHi)<<4);

    //Convert to double from range
    bool isUnipolar = (bool)(this->analogRange & 1<<2);
    bool bipolarHalve = !(bool)(this->analogRange & 1<<3);
    uint8_t factorOfTen = (this->analogRange & 0x3);

    //map input bytes from 12bit to double
    double minVal = -5;
    double maxVal = 5;
    if (isUnipolar) {
        //0 -> 0V, 0xFFF or 4095 -> 10*10^(-factorOfTen)V
        minVal = 0;
        maxVal = pow(10.0, -factorOfTen+1);
    } else {
        if (bipolarHalve) {
            minVal = -5*pow(10.0, -factorOfTen);
            maxVal = 5*pow(10.0, -factorOfTen);
        } else {
            minVal = -10*pow(10.0, -factorOfTen);
            maxVal = 10*pow(10.0, -factorOfTen);
        }
    }
    return mapVal((double)hexInByte, MIN_12BIT, MAX_12BIT, minVal, maxVal);
}

std::ostream& operator<<(std::ostream& output, const EmbeddedDevice::PCM3718& pcm) {
    double ch0Volt = pcm.analogInput(0x00);
    double ch1Volt = pcm.analogInput(0x01);
    output << "channel 0: " << ch0Volt << "\tchannel 1: " << ch1Volt << std::endl;
    return output;
}
