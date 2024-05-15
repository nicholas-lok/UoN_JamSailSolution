/** Google Test Lib */
#include "gtest/gtest.h"

/** Standard Libs */
#include <thread>
#include <Windows.h>
#include <stdint.h>

/** Test Libs */
extern "C" {
	#include "../../lib/ttc/li2/Li2.h"
}

namespace Li2 {
    class GroundTest : public testing::Test {
    protected:  
        void SetUp() override {}

		Li2_Radio Li2;
		const char *serialPortName = "COM3";
		const uint32_t serialBaudRate = 9600;
		LI2_SETUP setup = { false, false, false, false, 0 };
    };



	TEST_F( GroundTest, Initialisation ) {


		EXPECT_EQ( LI2_OK, Li2_Init( &Li2, serialPortName, serialBaudRate, setup ) );
	}

}  // Li2 namespace

/**  */
int main( int argc, char *argv[] ) {
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}