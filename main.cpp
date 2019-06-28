
#include "EmbeddedOperations.h"
#include "PCM3718.h"
#include "MSIP404.h"
#include "DAC06.h"

int main(int argc, char **argv) {
    EmbeddedOperations eops;
    EmbeddedDevice::PCM3718 pcm( &eops, 0x300);
    EmbeddedDevice::MSIP404 msi( &eops, 0x200);
    EmbeddedDevice::DAC06 dac( &eops, 0x320);

    pcm.digitalOutput(1<<3|1<<6|1<<10);
    std::cout << "Digital Read " << pcm.digitalInput() << std::endl;
    std::cout << "Digital Read " << pcm.digitalByteInput(false) << std::endl;
    std::cout << "Digital Read " << pcm.digitalByteInput(true) << std::endl;

    int32_t testVal = msi.readChannel(0);
    dac.analogOutputRaw(0, (uint16_t)(testVal & 0x00FF));

    pcm.setRange(1<<4);
    std::cout << "Analog Read " << pcm.analogInput(0) << std::endl;
}
