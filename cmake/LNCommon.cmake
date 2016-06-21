﻿
# Unicode char set.
#option(LN_USE_UNICODE_CHAR_SET "Use unicode char set." ON)

# MSVC Runtime library.
#option(LN_MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME "static runtime (/MT, /MTd) or DLL runtime (/MD, /MDd) or to link either." ON)

# Exceprion Backtrace.
#option(LN_EXCEPTION_BACKTRACE "In some unix environment there is a possibility that can not be compiled." ON)

set(LN_ARCH "x86")			# default
set(LN_TARGET_ENV "MSVC12")	# default

#------------------------------------------------------------------------------
# Options
#------------------------------------------------------------------------------
# Unicode char set.
if (LN_USE_UNICODE_CHAR_SET)
	add_definitions(-DUNICODE)
	add_definitions(-D_UNICODE)
endif()

if (WIN32)
	# ランタイムライブラリ
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
	
	# Architecture.
	# http://stackoverflow.com/questions/5334095/cmake-multiarchitecture-compilation
	if (${CMAKE_EXE_LINKER_FLAGS} MATCHES "/machine:x64")	# /machine:x64
		set(LN_ARCH "x64")
	else()
		set(LN_ARCH "x86")
	endif()
	
	# MSVC version
	if (MSVC_VERSION EQUAL 1400)
		set(LN_TARGET_ENV "MSVC80")
	elseif (MSVC_VERSION EQUAL 1500)
		set(LN_TARGET_ENV "MSVC90")
	elseif (MSVC_VERSION EQUAL 1600)
		set(LN_TARGET_ENV "MSVC100")
	elseif (MSVC_VERSION EQUAL 1700)
		set(LN_TARGET_ENV "MSVC110")
	elseif (MSVC_VERSION EQUAL 1800)
		set(LN_TARGET_ENV "MSVC120")
	elseif (MSVC_VERSION EQUAL 1900)
		set(LN_TARGET_ENV "MSVC140")
	endif()
endif()

#------------------------------------------------------------------------------
# functions
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# LN_SOURCES
# LN_OUTPUT_POSTFIX
# LN_INCLUDE_DIRECTORIES
macro(ln_add_executable projectName)
	add_executable(${projectName} ${LN_SOURCES})
	set_target_properties(${projectName} PROPERTIES OUTPUT_NAME ${projectName}${LN_OUTPUT_POSTFIX})
	set_target_properties(${projectName} PROPERTIES DEBUG_POSTFIX "d")
	set_target_properties(${projectName} PROPERTIES INCLUDE_DIRECTORIES "${LN_INCLUDE_DIRECTORIES}")
	add_custom_command(
		TARGET ${projectName}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${projectName}> "${CMAKE_SOURCE_DIR}/lib")
endmacro()

#------------------------------------------------------------------------------
# LN_SOURCES
# LN_OUTPUT_POSTFIX
# LN_INCLUDE_DIRECTORIES
# LN_LINK_LIBS
macro(ln_add_library projectName)
	add_library(${projectName}_Static STATIC ${LN_SOURCES})
	set_target_properties(${projectName}_Static PROPERTIES INCLUDE_DIRECTORIES "${LN_INCLUDE_DIRECTORIES}")
	set_target_properties(${projectName}_Static PROPERTIES OUTPUT_NAME "${projectName}${LN_LIB_POSTFIX}")
	target_link_libraries(${projectName}_Static ${LN_LINK_LIBS})
	add_custom_command(TARGET ${projectName}_Static POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${projectName}_Static> "${CMAKE_SOURCE_DIR}/lib")
endmacro()

#------------------------------------------------------------------------------
function(ln_make_postfix_for_shared_lib outPostfix)
	set(postfix "")
	
	if (WIN32)
		
		# Architecture.
		# http://stackoverflow.com/questions/5334095/cmake-multiarchitecture-compilation
		#if (${CMAKE_EXE_LINKER_FLAGS} MATCHES "/machine:x64")	# /machine:x64
		#	set(postfix "${postfix}_x64")
		#else()
		#	set(postfix "${postfix}_x86")
		#endif()

		# Unicode.
		if (${LN_USE_UNICODE_CHAR_SET})
			set(postfix "${postfix}U")
		endif()

		# MSVC Runtime library.
		#if (LN_MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME)
		#	set(postfix "${postfix}MT")
		#else()
		#	set(postfix "${postfix}MD")
		#endif()

		set(${outPostfix} ${postfix} PARENT_SCOPE)

	elseif (APPLE)
		# Unicode.
		if (${LN_USE_UNICODE_CHAR_SET})
			set(postfix "${postfix}U")
		endif()
		
		set(${outPostfix} ${postfix} PARENT_SCOPE)

	elseif (UNIX)
		# Unicode.
		if (${LN_USE_UNICODE_CHAR_SET})
			set(postfix "${postfix}U")
		endif()
		
		set(${outPostfix} ${postfix} PARENT_SCOPE)

	else()
		# Not support.
	    message(FATAL_ERROR "No supported platform was detected.")
	endif()
endfunction()

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
		elseif (MSVC_VERSION EQUAL 1900)
			set(postfix "${postfix}_msvc140")
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
function(ln_make_output_dir outPath)
	set(${outPath} "${CMAKE_SOURCE_DIR}/lib/${LN_TARGET_ENV}/${LN_ARCH}" PARENT_SCOPE)
endfunction()


# make output dir path
ln_make_output_dir(LN_LIB_OUTPUT_DIR)

