
#include "MSIP404.h"

uint8_t chsReadBases[8] = { 0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C }

MSIP404(EmbeddedOperations *eops, uint32_t base_addr) {
    //Initisalise base vals
    this->eops = eops;
    this->baseAddr = base_addr;
    
    //check perms
    if (this->eops->ioperm(this->baseAddr, 16, 1) != 0) {
        std::cout << "fail perm" << std::endl;
    }
    !this;
}

void resetChannel(uint8_t channel) {
    if (channel <= 0x07 && channel >= 0x00) {
        this->eops->outb(0xFF, this->baseAddr+channel);
    }
}
int32_t readChannel(uint8_t channel) {
    if (channel <= 0x07 && channel >= 0x00) {
        int32_t inLoByte =  (int32_t) this->eops->inb(this->baseAddr+chsReadBases[channel]);
        int32_t in2ndByte = (int32_t) this->eops->inb(this->baseAddr+chsReadBases[channel]+1);
        int32_t in3rdByte = (int32_t) this->eops->inb(this->baseAddr+chsReadBases[channel]+2);
        int32_t inHiByte =  (int32_t) this->eops->inb(this->baseAddr+chsReadBases[channel]+3);

        return inLoByte + (in2ndByte<<(sizeof(uint8_t))) + (in3rdByte<<(2*sizeof(uint8_t))) + (inHiByte<<(3*sizeof(uint8_t)));
    }
}
bool readIndex(uint8_t channel);
bool operator!() {
    for (uint8_t i=0; i<=7; i++) {
        this->resetChannel(i);
    }
    return 0;
}
