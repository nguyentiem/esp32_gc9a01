set(srcs
         "gc9a01.c"
         "main.c"
         "file_system_util.c"
         "oled.c"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")


