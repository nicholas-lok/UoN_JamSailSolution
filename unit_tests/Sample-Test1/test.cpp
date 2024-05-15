/** Google Test Lib */
#include "gtest/gtest.h"

/** Standard Libs */
#include <thread>
#include <Windows.h>

/** Test Libs */
extern "C" {
    #include "../../lib/obc/config/config.h"
    #include "../../lib/obc/transfer_buff/transfer_buff.h"
    #include "../../lib/ttc/twsg/TWSG_Ground.h"
    #include "../../lib/ttc/twsg/TWSG_Space.h"
    #include "../../lib/utils/win/img_reader/img_reader.h"
}

namespace TWSG {

    /**  */
    TWSG_Comms_System TWSG_Ground;
    TWSG_Comms_System TWSG_Space;
    Config Config_Ground;
    Config Config_Space;
    Transfer_Buff TransferBuff_Ground;
    Transfer_Buff TransferBuff_Space;

    /**  */
    void sendBeaconRoutine() {
        /** */
        TWSG_Init( &TWSG_Space, Config_Space );

        //Sleep( 2000 );

        /** */
        TWSG_OBC_Beacon beacon;
        memset( &beacon, 0, 64 );
        beacon.callSign[0] = 254;
        beacon.checksum = 4000;
        EXPECT_EQ( 0, TWSG_SendBeacon( &TWSG_Space, &beacon ) );
    }

    /**  */
    void receiveBeaconRoutine() {
        /** */
        TWSG_Init( &TWSG_Ground, "Config_Ground.ini" );

        /** */
        EXPECT_EQ( 0, TWSG_WaitForPacket( &TWSG_Ground, TWSG_OBC_BEACON ) );
    }

    /**  */
    void sendImageRoutine() {
        /** */
        if ( Config_Init( &Config_Space, "../../config/Config_Space.ini" ) != 0 ) abort();

        /** */
        TWSG_Init( &TWSG_Space, Config_Space );

        /** */
        EXPECT_EQ( 0, IR_Read( "../../../TestingData/testImage.tif", TWSG_Space.transferBuffer, , &TWSG_Space.dataInBufferSize ) );

        /** */
        TWSG_OBC_Buff_Down_Payload payload;
        payload.Telemetry.Meta.fileType = 1;
        payload.Telemetry.Meta.fileHash = 1234;
        payload.Telemetry.Meta.totalPacketCount = TWSG_Space.dataInBufferSize;
        EXPECT_EQ( 0, TWSG_SendBuffer( &TWSG_Space, &payload ) );
    }

    /**  */
    void receiveImageRoutine() {
        /** */
        if ( Config_Init( &Config_Ground, "../../config/TWSG_Config_Ground.ini" ) != 0 ) abort();

        /** */
        TWSG_Init( &TWSG_Ground, "TWSG_Config_Ground.ini" );

        /** */
        TWSG_OBC_Buff_Down_Payload payload;
        EXPECT_EQ( 0, TWSG_ReceiveBuffDown( &TWSG_Ground, &payload ) );

        /** */
        EXPECT_EQ( 0, IR_Write( "../../../TestingData/testImage_Cpy.tif", TWSG_Ground.transferBuffer, TWSG_Ground.dataInBufferSize ) );
    }



    /**  */
    TEST( Ground, Initialisation ) {
        ASSERT_EQ( 0, TWSG_Init( &TWSG_Ground, "TWSG_Config_Ground.ini" ) );
    }

    /**  */
    TEST( Space, Initialisation ) {
        ASSERT_EQ( 0, TWSG_Init( &TWSG_Space, "TWSG_Config_Space.ini" ) );
    }

    /**  */
    TEST( FullLink, Beacon ) {
        /**  */
        std::thread t1( sendBeaconRoutine );
        std::thread t2( receiveBeaconRoutine );
        t1.join();
        t2.join();
    }

    /**  */
    TEST( FullLink, Image ) {
        /**  */
        std::thread t1( sendImageRoutine );
        std::thread t2( receiveImageRoutine );
        t1.join();
        t2.join();

        /**  */
        char *img = ( char * )malloc( TWSG_Ground.dataInBufferSize );
        char *imgCpy = ( char * )malloc( TWSG_Ground.dataInBufferSize );

        /**  */
        memcpy( img, TWSG_Ground.transferBuff, TWSG_Ground.dataInBufferSize );
        memcpy( imgCpy, TWSG_Space.transferBuff, TWSG_Ground.dataInBufferSize );

        /**  */
        ASSERT_STREQ( img, imgCpy );
    }

}  // TWSG namespace

/**  */
int main( int argc, char *argv[] ) {
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}