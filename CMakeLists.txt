cmake_minimum_required(VERSION 3.23)

project(halolog VERSION 0.0.1 LANGUAGES CXX)

add_library(halolog INTERFACE)
add_library(halolog::halolog ALIAS halolog)

target_compile_features(halolog INTERFACE cxx_std_20)

include(GNUInstallDirs)

target_include_directories(halolog
	INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# MSVC: conformant preprocessor (required for __VA_OPT__) + UTF-8 source
target_compile_options(halolog INTERFACE
	$<$<CXX_COMPILER_ID:MSVC>:/utf-8 /Zc:preprocessor>
)

if(PROJECT_IS_TOP_LEVEL)
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX
			"${CMAKE_BINARY_DIR}/install"
			CACHE PATH "Default installation directory"
			FORCE
		)
	endif()

	include(CMakePackageConfigHelpers)

	file(GLOB_RECURSE HALOLOG_HEADERS
		CONFIGURE_DEPENDS
		include/*.h
	)

	target_sources(halolog INTERFACE
		FILE_SET HEADERS
		BASE_DIRS include
		FILES ${HALOLOG_HEADERS}
	)

	install(
		TARGETS halolog
		EXPORT halologTargets
		FILE_SET HEADERS
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)

	install(
		EXPORT halologTargets
		FILE halologTargets.cmake
		NAMESPACE halolog::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/halolog
	)

	write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/halologConfigVersion.cmake"
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion
	)

	configure_package_config_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/halologConfig.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/halologConfig.cmake"
		INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/halolog
	)

	install(
		FILES
		"${CMAKE_CURRENT_BINARY_DIR}/halologConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/halologConfigVersion.cmake"
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/halolog
	)
endif()

option(HALOLOG_BUILD_EXAMPLES "Build Halolog usage examples" ${PROJECT_IS_TOP_LEVEL})
option(HALOLOG_BUILD_QT_EXAMPLE "Build the Qt usage example" OFF)
option(HALOLOG_BUILD_TESTS "Build Halolog test" ${PROJECT_IS_TOP_LEVEL})

if(HALOLOG_BUILD_EXAMPLES)
	add_subdirectory(example)
endif()

if(HALOLOG_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()
