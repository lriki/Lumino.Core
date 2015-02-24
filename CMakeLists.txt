# e.g.) $ cmake -DLN_EXCEPTION_BACKTRACE=OFF ..


cmake_minimum_required (VERSION 2.8.7)
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

# Exceprion Backtrace.
option(LN_EXCEPTION_BACKTRACE "In some unix environment there is a possibility that can not be compiled." ON)

# Architecture.
#option(LN_BUILD_X64 "Architecture." OFF)

#------------------------------------------------------------------------------
# Apply options
#------------------------------------------------------------------------------
# Unicode char set.
if (LN_USE_UNICODE_CHAR_SET)
	add_definitions(-DUNICODE)
	add_definitions(-D_UNICODE)
endif()

find_package(OpenGL REQUIRED)

if (WIN32)
	# MSVC Version name
	if (MSVC_VERSION EQUAL 1400)
		set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}_msvc80")
	elseif (MSVC_VERSION EQUAL 1500)
		set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}_msvc90")
	elseif (MSVC_VERSION EQUAL 1600)
		set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}_msvc100")
	elseif (MSVC_VERSION EQUAL 1700)
		set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}_msvc110")
	elseif (MSVC_VERSION EQUAL 1800)
		set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}_msvc120")
	endif()
	
	# Architecture.
	# http://stackoverflow.com/questions/5334095/cmake-multiarchitecture-compilation
	if (${CMAKE_EXE_LINKER_FLAGS} MATCHES "/machine:x64")	# /machine:x64 
		set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}x64")
	else()
		set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}x86")
	endif()
	
	set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}_static")
	
	# MSVC Runtime library.
	if (MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME)
		set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}_MD")
	else()
		set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}_MT")
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
	set(CMAKE_DEBUG_POSTFIX "d")
	
	# link librarys.
	list(APPEND LN_LINK_LIBS "imagehlp.lib")
	
	# platform sources.
	set(LN_PLATFORM_SOURCES
		src/IO/Win32/FileUtils_win32.cpp
		src/Platform/Win32/Environment.cpp
		src/Platform/Win32/Win32WindowManager.cpp
		src/Platform/Win32/Win32WindowBase.cpp
		src/Platform/Win32/Win32Window.cpp)

elseif (APPLE)
	set(LN_UNIT_TEST_LINK_LIBS "pthread")
	
elseif (UNIX)
	set(LN_UNIT_TEST_LINK_LIBS "pthread")
	
	# link librarys.
	list(APPEND LN_LINK_LIBS "dl")
	list(APPEND LN_LINK_LIBS "bfd")
    find_package(X11 REQUIRED)

    #list(APPEND glfw_INCLUDE_DIRS ${X11_X11_INCLUDE_PATH})
    list(APPEND LN_LINK_LIBS ${X11_X11_LIB})

	#list(APPEND glfw_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
    list(APPEND LN_LINK_LIBS ${OPENGL_gl_LIBRARY})

	# platform sources.
	set(LN_PLATFORM_SOURCES
		src/IO/Unix/FileUtils_unix.cpp
		src/Platform/Environment_unix.cpp
		src/Platform/X11/X11WindowManager.cpp
		src/Platform/X11/X11Window.cpp)
	
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
	src/Text/ASCIIEncoding.cpp
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
	src/Platform/WindowManagerBase.cpp
	src/Platform/WindowBase.cpp
	src/Platform/PlatformManager.cpp
	src/Testing/TestUtils.cpp
	${LN_PLATFORM_SOURCES})

#------------------------------------------------------------------------------
# Output
#------------------------------------------------------------------------------

#set(CMAKE_CXX_FLAGS "")
#set(CMAKE_CXX_FLAGS_DEBUG "-g -pg")
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -pg -O3")
#set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")
#set(CMAKE_CXX_FLAGS_MINSIZEREL "")

if (LN_EXCEPTION_BACKTRACE)
	add_definitions(-DLN_EXCEPTION_BACKTRACE)
endif()

#------------------------------
# Static library.
add_library(LuminoCore_Static STATIC ${LN_MATH_SOURCES})
set_target_properties(LuminoCore_Static PROPERTIES OUTPUT_NAME ${LN_STATIC_LIB_OUTPUT_NAME})
#set_target_properties(LuminoCore_Static PROPERTIES DEBUG_POSTFIX "_d")
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


# Unit test.
add_subdirectory(test)

# samples.
#add_subdirectory(sample)
