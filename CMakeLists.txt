cmake_minimum_required (VERSION 2.8.4)
project (LuminoCore)

set(LN_STATIC_LIB_OUTPUT_NAME "LuminoCore")
set(LN_LINK_LIBS "")
set(LN_UNIT_TEST_LINK_LIBS "")

#------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------

# Unicode char set.
option(LN_USE_UNICODE_CHAR_SET "Use unicode char set." ON)

# MSVC Runtime library.
option(MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME "static runtime (/MT, /MTd) or DLL runtime (/MD, /MDd) or to link either." ON)

#------------------------------------------------------------------------------
# Apply options
#------------------------------------------------------------------------------
# Unicode char set.
if (LN_USE_UNICODE_CHAR_SET)
	add_definitions(-DUNICODE)
	add_definitions(-D_UNICODE)
endif()

if (WIN32)
	# MSVC Version name
	if (MSVC_VERSION EQUAL 1400)
		string(CONCAT LN_STATIC_LIB_OUTPUT_NAME ${LN_STATIC_LIB_OUTPUT_NAME} "-msvc80")
	elseif (MSVC_VERSION EQUAL 1500)
		string(CONCAT LN_STATIC_LIB_OUTPUT_NAME ${LN_STATIC_LIB_OUTPUT_NAME} "-msvc90")
	elseif (MSVC_VERSION EQUAL 1600)
		string(CONCAT LN_STATIC_LIB_OUTPUT_NAME ${LN_STATIC_LIB_OUTPUT_NAME} "-msvc100")
	elseif (MSVC_VERSION EQUAL 1700)
		string(CONCAT LN_STATIC_LIB_OUTPUT_NAME ${LN_STATIC_LIB_OUTPUT_NAME} "-msvc110")
	elseif (MSVC_VERSION EQUAL 1800)
		string(CONCAT LN_STATIC_LIB_OUTPUT_NAME ${LN_STATIC_LIB_OUTPUT_NAME} "-msvc120")
	endif()
	
	# MSVC Runtime library.
	if (MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME)
		#string(CONCAT LN_STATIC_LIB_OUTPUT_NAME ${LN_STATIC_LIB_OUTPUT_NAME} "-MD")
	else()
		#string(CONCAT LN_STATIC_LIB_OUTPUT_NAME ${LN_STATIC_LIB_OUTPUT_NAME} "-MT")
		# Overwrite options.
		foreach (flag
			CMAKE_C_FLAGS
			CMAKE_C_FLAGS_DEBUG
			CMAKE_C_FLAGS_RELEASE
			CMAKE_CXX_FLAGS
			CMAKE_CXX_FLAGS_DEBUG
			CMAKE_CXX_FLAGS_RELEASE)
			if (${flag} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
			endif()
			if (${flag} MATCHES "/MDd")
				string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
			endif()
		endforeach()
	endif()
	
	# link librarys.
	list(APPEND LN_LINK_LIBS "imagehlp.lib")
	
	# platform sources.
	set(LN_PLATFORM_SOURCES
		src/Platform/Win32/Environment.cpp)

elseif (APPLE)
	set(LN_UNIT_TEST_LINK_LIBS "pthread")
	
elseif (UNIX)
	set(LN_UNIT_TEST_LINK_LIBS "pthread")
	
	# link librarys.
	list(APPEND LN_LINK_LIBS "dl")
	list(APPEND LN_LINK_LIBS "bfd")
	
	# platform sources.
	set(LN_PLATFORM_SOURCES
		src/IO/Unix/FileUtils_unix.cpp
		src/Platform/Environment_unix.cpp)
	
else()
	# Not support.
    message(FATAL_ERROR "No supported platform was detected.")
endif()

#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------
#------------------------------
# Source files.
set(LN_MATH_SOURCES
	src/Base/Exception.cpp
	src/Base/Memory.cpp
	src/Base/RefObject.cpp
	src/Base/String.cpp
	src/Base/StringUtils.cpp
	src/IO/DirectoryUtils.cpp
	src/IO/FileStream.cpp
	src/IO/FileUtils.cpp
	src/IO/PathName.cpp
	src/IO/PathUtils.cpp
	src/Text/DBCSEncoding.cpp
	src/Text/UTF16Encoding.cpp
	src/Text/UTF32Encoding.cpp
	src/Text/Encoding.cpp
	src/Text/EncodingDetector.cpp
	src/Text/UnicodeUtils.cpp
	src/Text/ConvertTable_Big5.c
	src/Text/ConvertTable_EUCKR.c
	src/Text/ConvertTable_GB2312.c
	src/Text/ConvertTable_SJIS.c
	src/Threading/ThreadingExceptions.cpp
	src/Threading/Thread.cpp
	src/Threading/Atomic.cpp
	src/Threading/EventFlag.cpp
	src/Threading/Mutex.cpp
	src/Threading/ReadWriteMutex.cpp
	${LN_PLATFORM_SOURCES})

#------------------------------
# Test source files.
set(LN_TEST_SOURCES
	test/gtest/src/gtest-all.cc
	test/gtest/src/gtest_main.cc
	test/UnitTest/Base/Test_Base_List.cpp
	test/UnitTest/Base/Test_Base_STLUtils.cpp
	test/UnitTest/Base/Test_Base_String.cpp
	test/UnitTest/Base/Test_Base_StringUtils.cpp
	test/UnitTest/Base/Test_Base_UnicodeUtils.cpp
	test/UnitTest/Base/Test_Base_Delegate.cpp
	test/UnitTest/Base/Test_Base_Encoding.cpp
	test/UnitTest/Base/Test_Base_Encoding_UTF8.cpp)

#------------------------------------------------------------------------------
# Output
#------------------------------------------------------------------------------


#------------------------------
# Static library.
set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}-static")
add_library(LuminoCore_Static STATIC ${LN_MATH_SOURCES})
set_target_properties(LuminoCore_Static PROPERTIES OUTPUT_NAME ${LN_STATIC_LIB_OUTPUT_NAME})
set_target_properties(LuminoCore_Static PROPERTIES DEBUG_POSTFIX "_d")
set_target_properties(
	LuminoCore_Static PROPERTIES INCLUDE_DIRECTORIES
	"${PROJECT_SOURCE_DIR}/include/Lumino")
target_link_libraries(LuminoCore_Static ${LN_LINK_LIBS})
add_custom_command(
	TARGET LuminoCore_Static
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LuminoCore_Static> "${PROJECT_SOURCE_DIR}/lib")

#------------------------------
# Shared library.
add_library(LuminoCore_Shared SHARED ${LN_MATH_SOURCES} ${LN_MATH_HEADERS})
set_target_properties(LuminoCore_Shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(LuminoCore_Shared PROPERTIES DEBUG_POSTFIX "_d")
set_target_properties(LuminoCore_Shared PROPERTIES COMPILE_DEFINITIONS "LUMINO_BUILD_DLL")
set_target_properties(
	LuminoCore_Shared PROPERTIES INCLUDE_DIRECTORIES
	"${PROJECT_SOURCE_DIR}/include/Lumino")
target_link_libraries(LuminoCore_Shared ${LN_LINK_LIBS})
add_custom_command(
	TARGET LuminoCore_Shared
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LuminoCore_Shared> "${PROJECT_SOURCE_DIR}/lib")
add_custom_command(
	TARGET LuminoCore_Shared
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_LINKER_FILE:LuminoCore_Shared> "${PROJECT_SOURCE_DIR}/lib")

#------------------------------
# Unit test.
add_executable(UnitTest ${LN_TEST_SOURCES})
set(LIST_OF_DIRECTORIES
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/test/gtest/include"
	"${PROJECT_SOURCE_DIR}/test/gtest"
	"${PROJECT_SOURCE_DIR}/test/UnitTest")
set_target_properties(
	UnitTest
	PROPERTIES INCLUDE_DIRECTORIES
	"${LIST_OF_DIRECTORIES}")
# gtest for VS2012.
set_target_properties(UnitTest PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_TR1_TUPLE=0")
target_link_libraries(UnitTest LuminoCore_Static ${LN_UNIT_TEST_LINK_LIBS})
add_dependencies(UnitTest LuminoCore_Static)

# samples.
#add_subdirectory(sample)
