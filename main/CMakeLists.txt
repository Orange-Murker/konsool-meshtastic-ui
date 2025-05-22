set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ON) # with newer cmake versions put all find_package in global scope
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    include(FetchContent)
    include(device-ui/cmake/protobuf.cmake)
    include(device-ui/cmake/nanopb.cmake)

    file(GLOB nanopb-src ${nanopb_SOURCE_DIR}/*.c)
endif()

file(GLOB_RECURSE device-ui-src ${PROJECT_DIR}/main/device-ui/src/* ${PROJECT_DIR}/main/device-ui/source/* ${PROJECT_DIR}/main/device-ui/generated/* ${PROJECT_DIR}/main/device-ui/locale/* ${PROJECT_DIR}/main/device-ui/generated/ui_320x240/*)

list(FILTER device-ui-src EXCLUDE REGEX "DisplayDriverFactory.cpp")

idf_component_register(
	SRCS
		"main.cpp"
		"BadgeBspDisplay.cpp"
		"Meshtastic.cpp"
		"EspClient.cpp"
		${device-ui-src}
		${nanopb-src}
	PRIV_REQUIRES
		esp_lcd
		fatfs
		nvs_flash
		badge-bsp
		SdFat
	INCLUDE_DIRS
	    "."
		"device-ui"
		"device-ui/src"
		"device-ui/include"
		"device-ui/locale"
		"device-ui/generated/ui_320x240"
		${GENERATED_FILES_DIR}
		${nanopb_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
)

add_compile_definitions(HAS_TFT VIEW_320x240 DEBUG_UNIT_TEST)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

idf_component_get_property(lvgl_lib lvgl COMPONENT_LIB)
idf_component_get_property(libpng_lib espressif__libpng COMPONENT_LIB)
target_link_libraries(${lvgl_lib} PUBLIC ${libpng_lib})

target_link_libraries(${COMPONENT_LIB} PRIVATE Protobufs)
