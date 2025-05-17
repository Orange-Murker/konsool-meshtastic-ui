if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    include(FetchContent)
    include(device-ui/cmake/protobuf.cmake)
    include(device-ui/cmake/nanopb.cmake)
endif()

idf_component_register(
	SRCS
		"main.cpp"
	PRIV_REQUIRES
		esp_lcd
		fatfs
		nvs_flash
		badge-bsp
	INCLUDE_DIRS
		"device-ui"
		"device-ui/src"
		"device-ui/locale"
		${GENERATED_FILES_DIR}
		${nanopb_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
)

idf_component_get_property(lvgl_lib lvgl COMPONENT_LIB)
idf_component_get_property(libpng_lib espressif__libpng COMPONENT_LIB)
target_link_libraries(${lvgl_lib} PUBLIC ${libpng_lib})

target_link_libraries(${COMPONENT_LIB} PRIVATE Protobufs)
