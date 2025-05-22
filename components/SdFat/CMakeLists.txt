file(GLOB_RECURSE src ${COMPONENT_DIR}/SdFat/src/*.cpp)

idf_component_register(
    SRCS
        ${src}
    INCLUDE_DIRS
        "SdFat/src"
    REQUIRES
        arduino-esp32
)
