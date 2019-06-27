
#include "EmbeddedOperations.h"
#include "PCM3718.h"
//#include "MSIP404.h"
//#include "DAC06.h"

int main(int argc, char **argv) {
    EmbeddedOperations eops;
    EmbeddedDevice::PCM3718 pcm( &eops, 0x300);
    //EmbeddedDevice::MSIP404 msi( &eops, 0x200);
    //EmbeddedDevice::DAC06 dac( &eops, 0x320);
}
