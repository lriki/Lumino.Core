﻿# e.g.) $ cmake -DLN_EXCEPTION_BACKTRACE=OFF ..

cmake_minimum_required (VERSION 3.0.0)
project (LuminoCore)

include(cmake/LNCommon.cmake NO_POLICY_SCOPE)

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
	include/Lumino/Base/ByteBuffer.h
	include/Lumino/Base/Cache.h
	include/Lumino/Base/Collection.h
	include/Lumino/Base/Common.h
	include/Lumino/Base/CRT.h
	include/Lumino/Base/Delegate.h
	include/Lumino/Base/Delegate.inl
	include/Lumino/Base/Diff.h
	include/Lumino/Base/DllLoader.h
	include/Lumino/Base/ElapsedTimer.h
	include/Lumino/Base/Enumerable.h
	include/Lumino/Base/EnumExtension.h
	include/Lumino/Base/Environment.h
	include/Lumino/Base/Event.h
	include/Lumino/Base/Event.inl
	include/Lumino/Base/Exception.h
	include/Lumino/Base/Formatter.h
	include/Lumino/Base/Hash.h
	include/Lumino/Base/List.h
	include/Lumino/Base/List.inl
	include/Lumino/Base/Locale.h
	include/Lumino/Base/Logger.h
	include/Lumino/Base/Memory.h
	include/Lumino/Base/Nullable.h
	include/Lumino/Base/RefObject.h
	include/Lumino/Base/Regex.h
	include/Lumino/Base/Resource.h
	include/Lumino/Base/Resource.h
	include/Lumino/Base/Serialization.h
	include/Lumino/Base/Stack.h
	include/Lumino/Base/StlHelper.h
	include/Lumino/Base/String.h
	include/Lumino/Base/StringArray.h
	include/Lumino/Base/StringBuilder.h
	include/Lumino/Base/StringRef.h
	include/Lumino/Base/StringHelper.h
	include/Lumino/Base/Typedef.h
	include/Lumino/Base/Version.h
	src/Base/ByteBuffer.cpp
	src/Base/Cache.cpp
	src/Base/Diff.cpp
	src/Base/DllLoader.cpp
	src/Base/ElapsedTimer.cpp
	src/Base/Environment.cpp
	src/Base/Exception.cpp
	src/Base/Hash.cpp
	src/Base/Locale.cpp
	src/Base/Logger.cpp
	src/Base/Memory.cpp
	src/Base/RefObject.cpp
	src/Base/Resource.cpp
	src/Base/Serialization.cpp
	src/Base/String.cpp
	src/Base/StringBuilder.cpp
	src/Base/StringRef.cpp
	src/Base/StringHelper.cpp
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
	src/IO/FileFinder_UNIX.h
	src/IO/FileFinder_Win32.h
	src/IO/FileStream.cpp
	src/IO/FileSystem.cpp
	src/IO/FileSystem_Unix.h
	src/IO/FileSystem_Win32.h
	src/IO/MemoryStream.cpp
	src/IO/PathName.cpp
	src/IO/PathTraits.cpp
	src/IO/Process.cpp
	src/IO/Process_Unix.h
	src/IO/Process_Win32.h
	src/IO/StreamReader.cpp
	src/IO/StreamWriter.cpp
	src/IO/StringReader.cpp
	src/IO/StringWriter.cpp
	src/IO/TextWriter.cpp
)

set(LN_SOURCES_Json
	include/Lumino/Json/Common.h
	include/Lumino/Json/JsonDiagnostics.h
	include/Lumino/Json/JsonDocument.h
	include/Lumino/Json/JsonHandler.h
	include/Lumino/Json/JsonReader.h
	include/Lumino/Json/JsonValue.h
	include/Lumino/Json/JsonWriter.h
	src/Json/JsonDiagnostics.cpp
	src/Json/JsonDocument.cpp
	src/Json/JsonHandler.cpp
	src/Json/JsonReader.cpp
	src/Json/JsonValue.cpp
	src/Json/JsonWriter.cpp
)

set(LN_SOURCES_Threading
	include/Lumino/Threading/AtomicCounter.h
	include/Lumino/Threading/ConditionFlag.h
	include/Lumino/Threading/Mutex.h
	include/Lumino/Threading/ReadWriteMutex.h
	include/Lumino/Threading/Semaphore.h
	include/Lumino/Threading/Task.h
	include/Lumino/Threading/TaskScheduler.h
	include/Lumino/Threading/Thread.h
	include/Lumino/Threading/ThreadingExceptions.h
	src/Threading/AtomicCounter.cpp
	src/Threading/ConditionFlag.cpp
	src/Threading/ConditionFlag_POSIX.h
	src/Threading/ConditionFlag_Win32.h
	src/Threading/Mutex.cpp
	src/Threading/Mutex_POSIX.h
	src/Threading/Mutex_Win32.h
	src/Threading/ReadWriteMutex.cpp
	src/Threading/ReadWriteMutex_POSIX.h
	src/Threading/ReadWriteMutex_Win32.h
	src/Threading/Semaphore.cpp
	src/Threading/Semaphore_POSIX.h
	src/Threading/Semaphore_Win32.h
	src/Threading/Task.cpp
	src/Threading/TaskScheduler.cpp
	src/Threading/Thread.cpp
	src/Threading/Thread_POSIX.h
	src/Threading/Thread_Win32.h
	src/Threading/ThreadingExceptions.cpp
)

set(LN_SOURCES_Xml
	include/Lumino/Xml/XmlDocument.h
	include/Lumino/Xml/XmlException.h
	include/Lumino/Xml/XmlReader.h
	include/Lumino/Xml/XmlWriter.h
	src/Xml/XmlDocument.cpp
	src/Xml/XmlException.cpp
	src/Xml/XmlReader.cpp
	src/Xml/XmlWriter.cpp
)

set(LN_SOURCES_Reflection
	include/Lumino/Reflection/Common.h
	include/Lumino/Reflection/EventArgsPool.h
	include/Lumino/Reflection/Notify.h
	include/Lumino/Reflection/Property.h
	include/Lumino/Reflection/PropertyMetadata.h
	include/Lumino/Reflection/ReflectionArrayObject.h
	include/Lumino/Reflection/ReflectionEventArgs.h
	include/Lumino/Reflection/ReflectionObject.h
	include/Lumino/Reflection/ReflectionStruct.h
	include/Lumino/Reflection/TypeInfo.h
	include/Lumino/Reflection/Variant.h
	src/Reflection/Notify.cpp
	src/Reflection/Property.cpp
	src/Reflection/ReflectionEventArgs.cpp
	src/Reflection/ReflectionObject.cpp
	src/Reflection/TypeInfo.cpp
	src/Reflection/Variant.cpp
)

set(LN_SOURCES_Math
	include/LuminoMath.h
	include/Lumino/Math/Geometries.h
	include/Lumino/Math/Quaternion.inl
	include/Lumino/Math/Random.h
	include/Lumino/Math/SQTTransform.h
	include/Lumino/Math/Vector2.h
	include/Lumino/Math/Vector2.inl
	include/Lumino/Math/Vector3.h
	include/Lumino/Math/Vector3.inl
	include/Lumino/Math/Vector4.h
	include/Lumino/Math/Vector4.inl
	include/Lumino/Math/Common.h
	include/Lumino/Math/MathUtils.h
	include/Lumino/Math/Matrix.h
	include/Lumino/Math/Matrix.inl
	include/Lumino/Math/Plane.h
	include/Lumino/Math/Plane.inl
	include/Lumino/Math/Quaternion.h
	include/Lumino/Math/Version.h
	include/Lumino/Math/ViewFrustum.h
	src/Math/Geometries.cpp
	src/Math/Random.cpp
	src/Math/SQTTransform.cpp
	src/Math/Vector2.cpp
	src/Math/Vector3.cpp
	src/Math/Vector4.cpp
	src/Math/MathUtils.cpp
	src/Math/Matrix.cpp
	src/Math/Plane.cpp
	src/Math/Quaternion.cpp
	src/Math/ViewFrustum.cpp
)

set(LN_SOURCES_Testing
	include/Lumino/Testing/TestHelper.h
	src/Testing/TestHelper.cpp
)

#--------------------------------------
# Platform source files.
if (WIN32)
	set(LN_SOURCES_Text
		${LN_SOURCES_Text}
		src/Text/Win32CodePageEncoding.cpp
		src/Text/Win32CodePageEncoding.h
	)

elseif (APPLE)

elseif (UNIX)

endif()

#--------------------------------------
# Grouping.
source_group("Base"			FILES ${LN_SOURCES_Base})
source_group("Text"			FILES ${LN_SOURCES_Text})
source_group("IO"			FILES ${LN_SOURCES_IO})
source_group("Json"			FILES ${LN_SOURCES_Json})
source_group("Threading"	FILES ${LN_SOURCES_Threading})
source_group("Xml"			FILES ${LN_SOURCES_Xml})
source_group("Reflection"	FILES ${LN_SOURCES_Reflection})
source_group("Testing"		FILES ${LN_SOURCES_Testing})

set(LN_SOURCES
	include/LuminoCore.h
	include/LuminoMath.h
	src/LuminoCore.PCH.cpp
	src/LuminoCore.PCH.h
	${LN_SOURCES_Base}
	${LN_SOURCES_Text}
	${LN_SOURCES_IO}
	${LN_SOURCES_Json}
	${LN_SOURCES_Threading}
	${LN_SOURCES_Xml}
	${LN_SOURCES_Reflection}
	${LN_SOURCES_Testing})

set(LN_INCLUDE_DIRECTORIES
	${PROJECT_SOURCE_DIR}/include)
include_directories(${LN_INCLUDE_DIRECTORIES})

#------------------------------------------------------------------------------
# Apply options
#------------------------------------------------------------------------------
# Unicode char set.
if (LN_USE_UNICODE_CHAR_SET)
	add_definitions(-DUNICODE)
	add_definitions(-D_UNICODE)
endif()

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
	
	add_definitions("/EHsc")	# VS2015, cmake

	# link librarys.
	list(APPEND LN_LINK_LIBS "imagehlp.lib")

elseif (APPLE)
	set(LN_UNIT_TEST_LINK_LIBS "pthread")
	set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
	FIND_LIBRARY(CARBON_LIBRARY Carbon)
	list(APPEND LN_LINK_LIBS ${CARBON_LIBRARY})

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
# Core
add_library(${PROJECT_NAME}_Static STATIC ${LN_SOURCES})
set_target_properties(${PROJECT_NAME}_Static PROPERTIES OUTPUT_NAME "${PROJECT_NAME}${LN_LIB_POSTFIX}")
target_link_libraries(${PROJECT_NAME}_Static ${LN_LINK_LIBS})
set_target_properties(${PROJECT_NAME}_Static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${LN_LIB_OUTPUT_DIR}")
ln_add_pch(${PROJECT_NAME}_Static "LuminoCore.PCH.h" "src/LuminoCore.PCH.cpp")

# Math
add_library(LuminoMath STATIC ${LN_SOURCES_Math})
set_target_properties(LuminoMath PROPERTIES OUTPUT_NAME "LuminoMath${LN_LIB_POSTFIX}")
target_link_libraries(LuminoMath ${LN_LINK_LIBS})
set_target_properties(LuminoMath PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${LN_LIB_OUTPUT_DIR}")

# Unit test.
add_subdirectory(test)

# samples.
#add_subdirectory(sample)
