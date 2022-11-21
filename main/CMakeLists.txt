idf_component_register(
    SRCS
        "./src/ringbuffer.cpp"
        "./src/espdelay.cpp"
        "./src/esp32button.cpp"
        "./src/espi2s.cpp"
        "./src/wifbnetwork.cpp"
        "./src/main.cpp"
    INCLUDE_DIRS
        "."
        "./inc"
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
        -Wno-missing-field-initializers
)

target_compile_definitions(${COMPONENT_LIB}
    PRIVATE
        _DEBUG=0
        DEFUALT_MODE_TRANSMIT=1
        NUM_CHANNELS=1
        SAMPLE_RATE=48000
        BITS_PER_SAMPLE=16
        RING_BUFFER_LENGTH=128
        RING_LENGTH=4
        RINGBUFF_AUTO_FIRST_ROTATE=1
)

target_include_directories(${COMPONENT_LIB}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(${COMPONENT_LIB}
    PRIVATE
        ${LIBRARIES}
)
