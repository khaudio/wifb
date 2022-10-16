idf_component_register(
    SRCS
        "./src/main.cpp"
        "./src/espdelay.cpp"
        "./src/esp32button.cpp"
        "./src/espi2s.cpp"
        "./src/wifbnetwork.cpp"
    INCLUDE_DIRS
        "."
        "./src"
        "./inc"
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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/ringbuffer)

list(APPEND LIBRARIES ringbuffer)

target_compile_options(${COMPONENT_LIB}
    PRIVATE
        -Wno-error
        -Wno-error=int-in-bool-context
        -Wshadow-compatible-local
        -fexceptions
        -fpermissive
        $<$<CONFIG:Debug>:-O0 -g -Wall>
        $<$<CONFIG:Release>:-O2>
        $<$<CONFIG:RelWithDebInfo>:-O2 -g>
)

target_compile_definitions(${COMPONENT_LIB}
    PRIVATE
        CONFIG_MODE_TRANSMIT=1
        $<$<CONFIG:Debug>:DEBUG_BUILD;_DEBUG>
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
