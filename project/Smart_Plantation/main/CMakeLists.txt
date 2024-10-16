idf_component_register(SRCS "mdns_server.c" "task_common.c" "adc_sensor.c" "main.c" "wifi-server.c" "temperature_sensor.c"
        INCLUDE_DIRS "."
        EMBED_FILES website/index.html website/app.css website/app.js website/favicon.ico)
