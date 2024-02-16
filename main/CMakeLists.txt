idf_component_register(
    SRCS
        "./src/ringbuffer.cpp"
        "./src/multibuffer.cpp"
        "./src/espdelay.cpp"
        "./src/esp32button.cpp"
        "./src/espi2s.cpp"
        "./src/wifbnetwork.cpp"
        "./src/wifbmetadata.cpp"
        "./src/main.cpp"
    INCLUDE_DIRS
        "."
        "./inc"
        "./lib"
        "./src"
    REQUIRES
        freertos
        esp_common
        esp_system
        esp_timer
        esp_wifi
        driver
        soc
        nvs_flash
        log
)

list(APPEND LIBRARIES)

target_compile_options(${COMPONENT_LIB}
    PRIVATE
        -Wall
        -fexceptions
        -Wshadow
        -Wno-error
        -Wno-error=unused-value
        -Wno-missing-field-initializers
)

target_compile_definitions(${COMPONENT_LIB}
    PRIVATE
        _DEBUG=1
        DEFUALT_MODE_TRANSMIT=1
        NUM_CHANNELS=1
        SAMPLE_RATE=48000
        BITS_PER_SAMPLE=8
        RING_BUFFER_LENGTH=64
        RING_LENGTH=4
        RINGBUFF_AUTO_FIRST_ROTATE=1
        METADATA_SIZE=32
        I2S_ENABLED=0
)

target_include_directories(${COMPONENT_LIB}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(${COMPONENT_LIB}
    PRIVATE
        ${LIBRARIES}
)
