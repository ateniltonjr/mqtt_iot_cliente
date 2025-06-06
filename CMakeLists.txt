
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(mqtt_iot_cliente C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(mqtt_iot_cliente 
               mqtt_iot_cliente.c
               inc/ssd1306.c
                )

# Inclui a pasta include
include_directories(${CMAKE_CURRENT_LIST_DIR}/inc)

# Adiciona as opções para incluir a pasta include como caminho de include para o compilador
# (garante que os headers em include/ sejam encontrados)
target_include_directories(mqtt_iot_cliente PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

pico_set_program_name(mqtt_iot_cliente "mqtt_iot_cliente")
pico_set_program_version(mqtt_iot_cliente "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(mqtt_iot_cliente 1)
pico_enable_stdio_usb(mqtt_iot_cliente 1)

# Add the standard library to the build
target_link_libraries(mqtt_iot_cliente
        pico_stdlib
        hardware_adc
        hardware_pio
        hardware_pwm
        hardware_i2c
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_mqtt
        pico_mbedtls
        pico_lwip_mbedtls)

# Add the standard include files to the build
target_include_directories(mqtt_iot_cliente PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(mqtt_iot_cliente       
        )

pico_add_extra_outputs(mqtt_iot_cliente)