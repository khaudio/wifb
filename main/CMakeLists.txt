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
        -fexceptions
        -fpermissive
        -Wshadow-compatible-local
        -Wno-error
        -Wno-error=sequence-point
        $<$<CONFIG:Debug>:-O0 -g -Wall>
        $<$<CONFIG:Release>:-O2>
)

target_compile_definitions(${COMPONENT_LIB}
    PRIVATE
        CONFIG_MODE_TRANSMIT=1
        RINGBUFF_AUTO_FIRST_ROTATE=1
        _DEBUG=0
        $<$<CONFIG:Debug>:DEBUG_BUILD;_DEBUG;>
        $<$<CONFIG:Release>:>
)

target_include_directories(${COMPONENT_LIB}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(${COMPONENT_LIB}
    PRIVATE
        ${LIBRARIES}
)
