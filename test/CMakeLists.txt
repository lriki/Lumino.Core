set(LN_TEST_SOURCES
	gtest/src/gtest-all.cc
	gtest/src/gtest_main.cc
	UnitTest/Base/Test_Base_Array.cpp
	UnitTest/Base/Test_Base_ByteBuffer.cpp
	UnitTest/Base/Test_Base_Delegate.cpp
	UnitTest/Base/Test_Base_Encoding.cpp
	UnitTest/Base/Test_Base_Encoding_UTF8.cpp
	UnitTest/Base/Test_Base_Exception.cpp
	UnitTest/Base/Test_Base_List.cpp
	UnitTest/Base/Test_Base_Rect.cpp
	UnitTest/Base/Test_Base_STLUtils.cpp
	UnitTest/Base/Test_Base_String.cpp
	UnitTest/Base/Test_Base_StringUtils.cpp
	UnitTest/Base/Test_Base_UnicodeUtils.cpp
	UnitTest/Base/Test_Base_UTF32Encoding.cpp
	UnitTest/IO/Test_IO_File.cpp
	UnitTest/IO/Test_IO_FileUtils.cpp
	UnitTest/IO/Test_IO_MemoryStream.cpp
	UnitTest/Text/Test_Text_DBCSEncoding.cpp
	UnitTest/Text/Test_Text_EncodingDetector.cpp
	UnitTest/Threading/Test_Threading_Atomic.cpp
	UnitTest/Threading/Test_Threading_EventFlag.cpp
	UnitTest/Threading/Test_Threading_Thread.cpp)

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/test/gtest/include"
	"${PROJECT_SOURCE_DIR}/test/gtest"
	"${PROJECT_SOURCE_DIR}/test/gtest/src"
	"${PROJECT_SOURCE_DIR}/test/UnitTest")

link_libraries(LuminoCore_Static)

add_definitions(-DGTEST_HAS_TR1_TUPLE=0)	# vs2012

add_executable(LuminoCore_UnitTest ${LN_TEST_SOURCES})
add_dependencies(LuminoCore_UnitTest LuminoCore_Static)
