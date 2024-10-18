idf_component_register(SRCS "peripherals/led_rgb.c" "backend/mdns_server.c" "common/task_common.c" "sensors/adc_read.c" "main.c" "backend/wifi-server.c" "sensors/temperature_sensor.c"
        INCLUDE_DIRS "."
        EMBED_FILES website/index.html website/app.css website/app.js website/favicon.ico)
