
#include "MSIP404.h"

#define INDEX_BASE_ADDRESS 0x800
#define EVEN_CH_INDEX_OFFSET 0x07
#define ODD_CH_INDEX_OFFSET 0x05

using namespace std;

EmbeddedDevice::MSIP404::MSIP404(EmbeddedOperations *eops, uint32_t base_addr) {
    //Initisalise base vals
    this->eops = eops;
    this->baseAddr = base_addr;
    
    //check perms
    if (this->eops->ioperm(this->baseAddr, 32, 1) | this->eops->ioperm(INDEX_BASE_ADDRESS, 2, 1)) {
        std::cout << "fail perm" << std::endl;
    }
    for (uint8_t i=0; i<=7; i++) {
        this->resetChannel(i);
    }
}

void EmbeddedDevice::MSIP404::resetChannel(uint8_t channel) {
    if (channel <= 0x07 && channel >= 0x00) {
        this->eops->outb(0xFF, this->baseAddr+channel);
    }
}
int32_t EmbeddedDevice::MSIP404::readChannel(uint8_t channel) {
    if (channel <= 0x07 && channel >= 0x00) {
        int32_t inLoByte =  (int32_t) this->eops->inb(this->baseAddr+channel*4);
        int32_t in2ndByte = (int32_t) this->eops->inb(this->baseAddr+channel*4+1);
        int32_t in3rdByte = (int32_t) this->eops->inb(this->baseAddr+channel*4+2);
        int32_t inHiByte =  (int32_t) this->eops->inb(this->baseAddr+channel*4+3);

        return inLoByte + (in2ndByte<<(sizeof(uint8_t))) + (in3rdByte<<(2*sizeof(uint8_t))) + (inHiByte<<(3*sizeof(uint8_t)));
    }
    return -1;
}

bool EmbeddedDevice::MSIP404::readIndex(uint8_t channel) {
    if (channel <= 0x02 && channel >= 0x00) {
        if (!channel%2) {
            return this->eops->inb(INDEX_BASE_ADDRESS+channel/2) & 1<<EVEN_CH_INDEX_OFFSET;
        } else {
            return this->eops->inb(INDEX_BASE_ADDRESS+(channel-1)/2) & 1<<ODD_CH_INDEX_OFFSET;
        }
    }
    return 0;
}

bool EmbeddedDevice::MSIP404::operator!() {
    for (uint8_t i=0; i<=7; i++) {
        this->resetChannel(i);
    }
    return 0;
}
