idf_component_register(
	SRCS
		"main.c"
	PRIV_REQUIRES
		esp_lcd
		fatfs
		nvs_flash
		badge-bsp
	INCLUDE_DIRS
		"."
)

idf_component_get_property(lvgl_lib lvgl COMPONENT_LIB)
idf_component_get_property(libpng_lib espressif__libpng COMPONENT_LIB)
target_link_libraries(${lvgl_lib} PUBLIC ${libpng_lib})
