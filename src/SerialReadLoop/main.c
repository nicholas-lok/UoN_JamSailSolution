#include "../../lib/src/libserialport/libserialport.h"
#include "../../lib/include/TWSG.h"
#include "../../lib/include/Li2_CDI.h"

#include <stdlib.h>

Li2_Radio Li2;

int main() {

    uint8_t RXBuffer[8] = { 0,0,0,0,0,0,0,0 };

    Serial_Init( &(Li2.SerialPort), "COM4", &Li2, 9600 );

    while (1) {
        if ( sp_input_waiting( Li2.SerialPort ) > 0 ) {

            printf( "%d----\n", sp_input_waiting( Li2.SerialPort) );

            sp_blocking_read( Li2.SerialPort, RXBuffer, 8, 1000 );

            for ( auto i = 0; i < 8; i++ ) {
                printf("%02x\n", RXBuffer[i]);
                RXBuffer[i] = 0;
            }
        }
    }
}