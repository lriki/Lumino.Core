
# Unicode char set.
#option(LN_USE_UNICODE_CHAR_SET "Use unicode char set." ON)

# MSVC Runtime library.
#option(LN_MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME "static runtime (/MT, /MTd) or DLL runtime (/MD, /MDd) or to link either." ON)

# Exceprion Backtrace.
#option(LN_EXCEPTION_BACKTRACE "In some unix environment there is a possibility that can not be compiled." ON)

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
endif()

#------------------------------------------------------------------------------
# functions
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
