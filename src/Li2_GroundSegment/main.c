#include "../../lib/src/libserialport/libserialport.h"
#include "../../lib/include/TWSG.h"
#include "../../lib/include/Li2_CDI.h"

Li2_Radio Li2;

int main() {
    Li2_Check( Li2_Init( &Li2, "COM5", 9600 ), "main" );

    for (auto i = 0; i < 8; i++) {
        printf("%02x\n", Li2.bufferOut[i]);
    }

    printf("-------------------\n");

    for (auto i = 0; i < 8; i++) {
        printf("%02x\n", Li2.bufferIn[i]);
    }

    printf("Waiting..............\n");

    return 0;
}

