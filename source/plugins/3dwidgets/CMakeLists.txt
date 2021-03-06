project(widgets3d)
project(pywidgets)

set(w3d_src
    widgets.cpp
    interactive.cpp
    translate_widgets.cpp
    )

include_directories(
            ${PROJECT_SOURCE_DIR}
            ${MINDTREE_SRC_LIB_DIR}
)

link_directories(
            ${PROJECT_SOURCE_DIR}
            ${PROJECT_BINARY_DIR}/source/lib
            ${MINDTREE_CORE_LIB_DIR}
            ${MINDTREE_SRC_LIB_DIR}
            ${MAIN_INCLUDE_DIR}
)

add_library(widgets3d SHARED ${w3d_src})
add_library(pywidgets SHARED python.cpp)

set_target_properties(widgets3d PROPERTIES PREFIX "")
set_target_properties(pywidgets PROPERTIES PREFIX "")
target_link_libraries(widgets3d
            render
            mindtree_core
            ${MINDTREE_CORE_LIB}
)

target_link_libraries(pywidgets widgets3d)

install(TARGETS widgets3d LIBRARY DESTINATION ${PROJECT_ROOT}/lib)
install(TARGETS pywidgets LIBRARY DESTINATION ${PROJECT_ROOT}/python/3dwidgets)
