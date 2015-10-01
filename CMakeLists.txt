# e.g.) $ cmake -DLN_EXCEPTION_BACKTRACE=OFF ..

cmake_minimum_required (VERSION 2.8.7)
project (LuminoCore)

set(LN_LINK_LIBS "")
set(LN_UNIT_TEST_LINK_LIBS "")
set(LN_LIB_POSTFIX "")

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
option(LN_MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME "static runtime (/MT, /MTd) or DLL runtime (/MD, /MDd) or to link either." ON)

# Exceprion Backtrace.
option(LN_EXCEPTION_BACKTRACE "In some unix environment there is a possibility that can not be compiled." ON)

# Architecture.
#option(LN_BUILD_X64 "Architecture." OFF)

#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------
#--------------------------------------
# Source files.
set(LN_SOURCES_Base
	include/Lumino/Base/Array.h
	include/Lumino/Base/Array.inl
	include/Lumino/Base/ByteBuffer.h
	include/Lumino/Base/Cache.h
	include/Lumino/Base/Collection.h
	include/Lumino/Base/Common.h
	include/Lumino/Base/CRT.h
	include/Lumino/Base/Delegate.h
	include/Lumino/Base/Delegate.inl
	include/Lumino/Base/DllLoader.h
	include/Lumino/Base/ElapsedTimer.h
	include/Lumino/Base/EnumExtension.h
	include/Lumino/Base/Environment.h
	include/Lumino/Base/Event.h
	include/Lumino/Base/Event.inl
	include/Lumino/Base/Exception.h
	include/Lumino/Base/Hash.h
	include/Lumino/Base/List.h
	include/Lumino/Base/Locale.h
	include/Lumino/Base/Logger.h
	include/Lumino/Base/Memory.h
	include/Lumino/Base/NonCopyable.h
	include/Lumino/Base/Nullable.h
	include/Lumino/Base/Point.h
	include/Lumino/Base/Queue.h
	include/Lumino/Base/Rect.h
	include/Lumino/Base/RefObject.h
	include/Lumino/Base/Resource.h
	include/Lumino/Base/Size.h
	include/Lumino/Base/SortedArray.h
	include/Lumino/Base/Stack.h
	include/Lumino/Base/STLUtils.h
	include/Lumino/Base/String.h
	include/Lumino/Base/StringBuilder.h
	include/Lumino/Base/StringTraits.h
	include/Lumino/Base/Thickness.h
	include/Lumino/Base/Typedef.h
	include/Lumino/Base/Version.h
	src/Base/ByteBuffer.cpp
	src/Base/Cache.cpp
	src/Base/DllLoader.cpp
	src/Base/ElapsedTimer.cpp
	src/Base/Environment.cpp
	src/Base/Exception.cpp
	src/Base/Hash.cpp
	src/Base/Locale.cpp
	src/Base/Logger.cpp
	src/Base/Memory.cpp
	src/Base/Point.cpp
	src/Base/Rect.cpp
	src/Base/RefObject.cpp
	src/Base/Resource.cpp
	src/Base/Size.cpp
	src/Base/String.cpp
	src/Base/StringBuilder.cpp
	src/Base/StringTraits.cpp
	src/Base/Thickness.cpp
	src/Base/Version.cpp
)
	
set(LN_SOURCES_Text
	include/Lumino/Text/Encoding.h
	include/Lumino/Text/EncodingConverter.h
	include/Lumino/Text/EncodingDetector.h
	include/Lumino/Text/UnicodeUtils.h
	src/Text/ASCIIEncoding.cpp
	src/Text/ASCIIEncoding.h
	src/Text/ConvertTable_Big5.c
	src/Text/ConvertTable_EUCJP_EXT.c
	src/Text/ConvertTable_EUCKR.c
	src/Text/ConvertTable_GB2312.c
	src/Text/ConvertTable_SJIS.c
	src/Text/DBCSEncoding.cpp
	src/Text/DBCSEncoding.h
	src/Text/Encoding.cpp
	src/Text/EncodingConverter.cpp
	src/Text/EncodingDetector.cpp
	src/Text/EUCJPEncoding.cpp
	src/Text/EUCJPEncoding.h
	src/Text/UnicodeUtils.cpp
	src/Text/UTF8Encoding.cpp
	src/Text/UTF8Encoding.h
	src/Text/UTF16Encoding.cpp
	src/Text/UTF16Encoding.h
	src/Text/UTF32Encoding.cpp
	src/Text/UTF32Encoding.h
)

set(LN_SOURCES_IO
	include/Lumino/IO/BinaryReader.h
	include/Lumino/IO/BinaryWriter.h
	include/Lumino/IO/Common.h
	include/Lumino/IO/Console.h
	include/Lumino/IO/DirectoryUtils.h
	include/Lumino/IO/File.h
	include/Lumino/IO/FileStream.h
	include/Lumino/IO/FileSystem.h
	include/Lumino/IO/MemoryStream.h
	include/Lumino/IO/PathName.h
	include/Lumino/IO/PathTraits.h
	include/Lumino/IO/Process.h
	include/Lumino/IO/Stream.h
	include/Lumino/IO/StreamReader.h
	include/Lumino/IO/StreamWriter.h
	include/Lumino/IO/StringReader.h
	include/Lumino/IO/StringWriter.h
	include/Lumino/IO/TextReader.h
	include/Lumino/IO/TextWriter.h
	src/IO/BinaryReader.cpp
	src/IO/BinaryWriter.cpp
	src/IO/Console.cpp
	src/IO/DirectoryUtils.cpp
	src/IO/File.cpp
	src/IO/FileStream.cpp
	src/IO/FileSystem.cpp
	src/IO/MemoryStream.cpp
	src/IO/PathName.cpp
	src/IO/PathTraits.cpp
	src/IO/Process.cpp
	src/IO/StreamReader.cpp
	src/IO/StreamWriter.cpp
	src/IO/StringReader.cpp
	src/IO/StringWriter.cpp
	src/IO/TextWriter.cpp
)

set(LN_SOURCES_Json
	include/Lumino/Json/Common.h
	include/Lumino/Json/JsonDocument.h
	include/Lumino/Json/JsonError.h
	include/Lumino/Json/JsonHandler.h
	include/Lumino/Json/JsonReader.h
	include/Lumino/Json/JsonValue.h
	include/Lumino/Json/JsonWriter.h
	src/Json/JsonDocument.cpp
	src/Json/JsonHandler.cpp
	src/Json/JsonReader.cpp
	src/Json/JsonValue.cpp
	src/Json/JsonWriter.cpp
)

set(LN_SOURCES_Threading
	include/Lumino/Threading/Atomic.h
	include/Lumino/Threading/EventFlag.h
	include/Lumino/Threading/Mutex.h
	include/Lumino/Threading/ReadWriteMutex.h
	include/Lumino/Threading/Thread.h
	include/Lumino/Threading/ThreadingExceptions.h
	src/Threading/ThreadingExceptions.cpp
	src/Threading/Thread.cpp
	src/Threading/Atomic.cpp
	src/Threading/EventFlag.cpp
	src/Threading/Mutex.cpp
	src/Threading/ReadWriteMutex.cpp
)

set(LN_SOURCES_Xml
	include/Lumino/Xml/XmlDocument.h
	include/Lumino/Xml/XmlError.h
	include/Lumino/Xml/XmlReader.h
	include/Lumino/Xml/XmlWriter.h
	src/Xml/XmlDocument.cpp
	src/Xml/XmlReader.cpp
	src/Xml/XmlWriter.cpp
)


set(LN_SOURCES_DataModel
	include/Lumino/Base/Event.h
	include/Lumino/Base/Event.inl
)

set(LN_SOURCES_Testing
	src/Testing/TestUtils.cpp
)

#--------------------------------------
# Platform source files.
if (WIN32)
	set(LN_SOURCES_IO
		${LN_SOURCES_IO}
		src/IO/FileSystem_Win32.cpp
		src/IO/Process_Win32.cpp)

	set(LN_SOURCES_Text
		${LN_SOURCES_Text}
		src/Text/Win32CodePageEncoding.cpp
		src/Text/Win32CodePageEncoding.h
	)	

elseif (APPLE)
	set(LN_SOURCES_IO
		${LN_SOURCES_IO}
		src/IO/FileSystem_Unix.cpp
		src/IO/Process_Unix.cpp)
	
elseif (UNIX)
	set(LN_SOURCES_IO
		${LN_SOURCES_IO}
		src/IO/FileSystem_Unix.cpp
		src/IO/Process_Unix.cpp)
		
endif()

#--------------------------------------
# Grouping.
source_group("Base"			FILES ${LN_SOURCES_Base})
source_group("Text"			FILES ${LN_SOURCES_Text})
source_group("IO"			FILES ${LN_SOURCES_IO})
source_group("Json"			FILES ${LN_SOURCES_Json})
source_group("Threading"	FILES ${LN_SOURCES_Threading})
source_group("Xml"			FILES ${LN_SOURCES_Xml})
source_group("Testing"		FILES ${LN_SOURCES_Testing})

set(LN_SOURCES 
	${LN_SOURCES_Base}
	${LN_SOURCES_Text}
	${LN_SOURCES_IO}
	${LN_SOURCES_Json}
	${LN_SOURCES_Threading}
	${LN_SOURCES_Xml}
	${LN_SOURCES_Testing})

set(LN_INCLUDE_DIRECTORIES
	${PROJECT_SOURCE_DIR}/include)
include_directories(${LN_INCLUDE_DIRECTORIES})

#------------------------------------------------------------------------------
# functions
#------------------------------------------------------------------------------
function(ln_make_postfix outPostfix)
	if (WIN32)
		#------------------------------------------------------
		# Make static lib name.
		#	{ProjectName}_{msvcVer}{Arch}_static_{Runtime}
		#	e.g)	LuminoMath_msvc120x86_static_MTd.lib
		if (MSVC_VERSION EQUAL 1400)
			set(postfix "${postfix}_msvc80")
		elseif (MSVC_VERSION EQUAL 1500)
			set(postfix "${postfix}_msvc90")
		elseif (MSVC_VERSION EQUAL 1600)
			set(postfix "${postfix}_msvc100")
		elseif (MSVC_VERSION EQUAL 1700)
			set(postfix "${postfix}_msvc110")
		elseif (MSVC_VERSION EQUAL 1800)
			set(postfix "${postfix}_msvc120")
		endif()
		
		# Architecture.
		# http://stackoverflow.com/questions/5334095/cmake-multiarchitecture-compilation
		if (${CMAKE_EXE_LINKER_FLAGS} MATCHES "/machine:x64")	# /machine:x64 
			set(postfix "${postfix}x64")
		else()
			set(postfix "${postfix}x86")
		endif()
		
		# Unicode.
		if (${LN_USE_UNICODE_CHAR_SET})
			set(postfix "${postfix}u")
		endif()
		
		set(postfix "${postfix}_static")
		
		# MSVC Runtime library.
		if (LN_MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME)
			set(postfix "${postfix}_MT")
		else()
			set(postfix "${postfix}_MD")
		endif()
		
		set(${outPostfix} ${postfix} PARENT_SCOPE)
		
		# end   Make static lib name.
		#------------------------------------------------------
		
	elseif (APPLE)
		set(${outPostfix} "" PARENT_SCOPE)
		
	elseif (UNIX)
		set(${outPostfix} "" PARENT_SCOPE)
		
	else()
		# Not support.
	    message(FATAL_ERROR "No supported platform was detected.")
	endif()
endfunction()

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
	if (LN_MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME)
		foreach (flag CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
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

elseif (APPLE)
	set(LN_UNIT_TEST_LINK_LIBS "pthread")
	set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
	
elseif (UNIX)
	set(LN_UNIT_TEST_LINK_LIBS "pthread")
	add_definitions("-std=c++11")
	
	# link librarys.
	list(APPEND LN_LINK_LIBS "dl")
	list(APPEND LN_LINK_LIBS "bfd")
	list(APPEND LN_LINK_LIBS "rt")
    find_package(X11 REQUIRED)

    #list(APPEND glfw_INCLUDE_DIRS ${X11_X11_INCLUDE_PATH})
    list(APPEND LN_LINK_LIBS ${X11_X11_LIB})

	#list(APPEND glfw_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
    list(APPEND LN_LINK_LIBS ${OPENGL_gl_LIBRARY})
	
else()
	# Not support.
    message(FATAL_ERROR "No supported platform was detected.")
endif()

# Make postfix.
ln_make_postfix(LN_LIB_POSTFIX)

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
set_target_properties(${PROJECT_NAME}_Static PROPERTIES OUTPUT_NAME "${PROJECT_NAME}${LN_LIB_POSTFIX}")
target_link_libraries(${PROJECT_NAME}_Static ${LN_LINK_LIBS})
add_custom_command(TARGET ${PROJECT_NAME}_Static POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}_Static> "${CMAKE_SOURCE_DIR}/lib")

# Unit test.
add_subdirectory(test)

# samples.
#add_subdirectory(sample)

