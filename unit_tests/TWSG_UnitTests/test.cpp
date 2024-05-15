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
    Config Config_Ground;
    Config Config_Space;
    Transfer_Buff TransferBuff_Ground;
    Transfer_Buff TransferBuff_Space;
    TWSG_Comms_System TWSG_Ground;
    TWSG_Comms_System TWSG_Space;

    void startGround() {
        /** */
        Config_Init( &Config_Ground, "../../config/Config_Ground.ini" );
        TransferBuff_Init( &TransferBuff_Ground, Config_Ground.TransferBuff.size );
        TWSG_Init( &TWSG_Ground, Config_Ground, &TransferBuff_Ground );
    }

    void startSpace() {
        /** */
        Config_Init( &Config_Space, "../../config/Config_Space.ini" );
        TransferBuff_Init( &TransferBuff_Space, Config_Space.TransferBuff.size );
        TWSG_Init( &TWSG_Space, Config_Space, &TransferBuff_Space );
    }

   

    /**  */
    TEST( Ground, Initialisation ) {
        /** */
        ASSERT_EQ( 0, Config_Init( &Config_Ground, "../../config/Config_Ground.ini" ) );
        ASSERT_EQ( 0, TransferBuff_Init( &TransferBuff_Ground, Config_Ground.TransferBuff.size ) );
        ASSERT_EQ( 0, TWSG_Init( &TWSG_Ground, Config_Ground, &TransferBuff_Ground ) );
    }

    /**  */
    TEST( Space, Initialisation ) {
        /** */
        ASSERT_EQ( 0, Config_Init( &Config_Space, "../../config/Config_Space.ini" ) );
        ASSERT_EQ( 0, TransferBuff_Init( &TransferBuff_Space, Config_Space.TransferBuff.size ) );
        ASSERT_EQ( 0, TWSG_Init( &TWSG_Space, Config_Space, &TransferBuff_Space ) );
    }

    /**  */
    void sendBeaconRoutine() {
        /** */
        startSpace();

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
        startGround();

        /** */
        EXPECT_EQ( 0, TWSG_WaitForPacket( &TWSG_Ground, TWSG_OBC_BEACON ) );
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
    void sendImageRoutine() {
        /** */
        startSpace();

        /** */
        EXPECT_EQ( 0, IR_Read( "../../../TestingData/testImage1.jpg", TransferBuff_Space.buff, TransferBuff_Space.maxSize , &TransferBuff_Space.dataInBuffSize ) );

        /** */
        TWSG_OBC_Buff_Down_Payload payload;
        payload.Telemetry.Meta.fileType = 1;
        payload.Telemetry.Meta.fileHash = 1234;
        payload.Telemetry.Meta.totalPacketCount = TransferBuff_Space.dataInBuffSize;
        EXPECT_EQ( 0, TWSG_SendBuffer( &TWSG_Space, &payload ) );
    }

    /**  */
    void receiveImageRoutine() {
        /** */
        startGround();

        /** */
        TWSG_OBC_Buff_Down_Payload payload;
        EXPECT_EQ( 0, TWSG_ReceiveBuffDown( &TWSG_Ground, &payload ) );

        /** */
        EXPECT_EQ( 0, IR_Write( "../../../TestingData/OUT.jpg", TransferBuff_Ground.buff, TransferBuff_Ground.dataInBuffSize ) );
    }

    /**  */
    TEST( FullLink, Image ) {
        /**  */
        std::thread t1( sendImageRoutine );
        std::thread t2( receiveImageRoutine );
        t1.join();
        t2.join();

        /**  */
        char *img = ( char * )malloc( TransferBuff_Ground.dataInBuffSize );
        char *imgCpy = ( char * )malloc( TransferBuff_Ground.dataInBuffSize );

        /**  */
        memcpy( img, TWSG_Ground.transferBuff->buff, TransferBuff_Ground.dataInBuffSize );
        memcpy( imgCpy, TWSG_Space.transferBuff->buff, TransferBuff_Ground.dataInBuffSize );

        /**  */
        ASSERT_STREQ( img, imgCpy );
    }

}  // TWSG namespace

/**  */
int main( int argc, char *argv[] ) {
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}