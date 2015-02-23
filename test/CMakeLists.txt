set(LN_TEST_SOURCES
	gtest/src/gtest-all.cc
	gtest/src/gtest_main.cc
	UnitTest/Base/Test_Base_List.cpp
	UnitTest/Base/Test_Base_STLUtils.cpp
	UnitTest/Base/Test_Base_String.cpp
	UnitTest/Base/Test_Base_StringUtils.cpp
	UnitTest/Base/Test_Base_UnicodeUtils.cpp
	UnitTest/Base/Test_Base_Delegate.cpp
	UnitTest/Base/Test_Base_Encoding.cpp
	UnitTest/Base/Test_Base_Encoding_UTF8.cpp
	UnitTest/Platform/Test_Platform_Application.cpp)

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/test/gtest/include"
	"${PROJECT_SOURCE_DIR}/test/gtest"
	"${PROJECT_SOURCE_DIR}/test/gtest/src"
	"${PROJECT_SOURCE_DIR}/test/UnitTest")

link_libraries(LuminoCore_Static)

add_definitions(-DGTEST_HAS_TR1_TUPLE=0)

#add_dependencies(UnitTest LuminoCore_Static)

add_executable(UnitTest ${LN_TEST_SOURCES})