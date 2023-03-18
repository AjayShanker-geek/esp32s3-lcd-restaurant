# set(LV_DEMO_DIR ../managed_components/lvgl__lvgl/demos)
# file(GLOB_RECURSE LV_DEMOS_SOURCES ${LV_DEMO_DIR}/*.c)

set(LV_DEMO_DIR widgets)
file(GLOB_RECURSE LV_DEMOS_SOURCES ${LV_DEMO_DIR}/*.c)

# set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)


# set(UI_PRINTER_DIR ./ui_printer)
# file(GLOB_RECURSE UI_PRINTER_SOURCES ${UI_PRINTER_DIR}/*.c)

# set(UI_TUNER_DIR ./ui_tuner)
# file(GLOB_RECURSE UI_TUNER_SOURCES ${UI_TUNER_DIR}/*.c)

# idf_component_register(
#     SRCS "main.c" ${LV_DEMOS_SOURCES} ${UI_PRINTER_SOURCES} ${UI_TUNER_SOURCES}
#     INCLUDE_DIRS "." ${LV_DEMO_DIR} ${UI_PRINTER_DIR}/include ${UI_TUNER_DIR}/include)

idf_component_register(
    SRCS "main.c" ${LV_DEMOS_SOURCES}
    INCLUDE_DIRS "." ${LV_DEMO_DIR} )
# ADD Ajay
# set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-variable -DLV_LVGL_H_INCLUDE_SIMPLE)
