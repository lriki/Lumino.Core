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
if (WIN32)
option(LN_USE_UNICODE_CHAR_SET "Use unicode char set." ON)
else()
option(LN_USE_UNICODE_CHAR_SET "Use unicode char set." OFF)
endif()

# MSVC Runtime library.
option(MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME "static runtime (/MT, /MTd) or DLL runtime (/MD, /MDd) or to link either." ON)

# Exceprion Backtrace.
option(LN_EXCEPTION_BACKTRACE "In some unix environment there is a possibility that can not be compiled." ON)

# Architecture.
#option(LN_BUILD_X64 "Architecture." OFF)

#option(LN_BUILD_SHARED_LIB "Build Shared lib." OFF)

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

	#------------------------------------------------------
	# Make static lib name.
	#	{ProjectName}_{msvcVer}{Arch}_static_{Runtime}
	#	e.g)	LuminoMath_msvc120x86_static_MTd.lib
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
	else()
		set(LN_STATIC_LIB_OUTPUT_NAME "${LN_STATIC_LIB_OUTPUT_NAME}_MD")
	endif()
	set(CMAKE_DEBUG_POSTFIX "d")
	# end   Make static lib name.
	#------------------------------------------------------
	
	# link librarys.
	list(APPEND LN_LINK_LIBS "imagehlp.lib")
	
	# platform sources.
	set(LN_PLATFORM_SOURCES
		src/IO/ArchiveMaker.cpp
		src/IO/Win32/FileUtils_win32.cpp
		src/Platform/Win32/Win32WindowManager.cpp
		src/Platform/Win32/Win32WindowBase.cpp
		src/Platform/Win32/Win32Window.cpp
		src/Platform/Win32/PlatformSupport_Win32.cpp)

elseif (APPLE)
	set(LN_UNIT_TEST_LINK_LIBS "pthread")
	
elseif (UNIX)
	set(LN_UNIT_TEST_LINK_LIBS "pthread")
	
	# link librarys.
	list(APPEND LN_LINK_LIBS "dl")
	list(APPEND LN_LINK_LIBS "bfd")
	list(APPEND LN_LINK_LIBS "rt")
    find_package(X11 REQUIRED)

    #list(APPEND glfw_INCLUDE_DIRS ${X11_X11_INCLUDE_PATH})
    list(APPEND LN_LINK_LIBS ${X11_X11_LIB})

	#list(APPEND glfw_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
    list(APPEND LN_LINK_LIBS ${OPENGL_gl_LIBRARY})

	# platform sources.
	set(LN_PLATFORM_SOURCES
		src/IO/Unix/FileUtils_unix.cpp
		src/Platform/X11/X11WindowManager.cpp
		src/Platform/X11/X11Window.cpp
		src/Platform/X11/PlatformSupport_X11.cpp)
	
else()
	# Not support.
    message(FATAL_ERROR "No supported platform was detected.")
endif()

#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------
#--------------------------------------
# Source files.
set(LN_SOURCES
	external/camellia/camellia.cpp
	src/Base/ByteBuffer.cpp
	src/Base/Cache.cpp
	src/Base/DllLoader.cpp
	src/Base/ElapsedTimer.cpp
	src/Base/Environment.cpp
	src/Base/Exception.cpp
	src/Base/Logger.cpp
	src/Base/Memory.cpp
	src/Base/RefObject.cpp
	src/Base/String.cpp
	src/Base/StringUtils.cpp
	src/Base/Version.cpp
	src/IO/Archive.cpp
	src/IO/ASyncIOObject.cpp
	src/IO/BinaryReader.cpp
	src/IO/DirectoryUtils.cpp
	src/IO/File.cpp
	src/IO/FileManager.cpp
	src/IO/FileStream.cpp
	src/IO/FileUtils.cpp
	src/IO/MemoryStream.cpp
	src/IO/PathName.cpp
	src/IO/PathUtils.cpp
	src/IO/File.cpp
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
	src/Platform/Window.cpp
	src/Platform/Application.cpp
	src/Testing/TestUtils.cpp
	${LN_PLATFORM_SOURCES})

set(LN_INCLUDE_DIRECTORIES
	${PROJECT_SOURCE_DIR}/include)
include_directories(${LN_INCLUDE_DIRECTORIES})

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

#--------------------------------------
# Static library.
add_library(${PROJECT_NAME}_Static STATIC ${LN_SOURCES})
set_target_properties(${PROJECT_NAME}_Static PROPERTIES OUTPUT_NAME ${LN_STATIC_LIB_OUTPUT_NAME})
target_link_libraries(${PROJECT_NAME}_Static ${LN_LINK_LIBS})
add_custom_command(TARGET ${PROJECT_NAME}_Static POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}_Static> "${PROJECT_SOURCE_DIR}/lib")

#--------------------------------------
# Shared library.
if (LN_BUILD_SHARED_LIB)
	add_library(LuminoCore_Shared SHARED ${LN_SOURCES})
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
endif()

# Unit test.
add_subdirectory(test)

# samples.
#add_subdirectory(sample)
