# include(ExternalProject)

set(_dir  ${CMAKE_CURRENT_SOURCE_DIR}/_GLider)
set(dir  ${CMAKE_CURRENT_SOURCE_DIR}/GLider)

if(NOT EXISTS ${dir})

    execute_process(COMMAND git clone https://github.com/swarnavaghosh04/GLider.git ${_dir})
    execute_process(COMMAND mkdir ${_dir}/build)
    execute_process(
        COMMAND cmake .. -DCMAKE_INSTALL_PREFIX=${dir}
        WORKING_DIRECTORY ${_dir}/build)
    execute_process(
        COMMAND cmake --build .
        WORKING_DIRECTORY ${_dir}/build)
    execute_process(
        COMMAND cmake --install .
        WORKING_DIRECTORY ${_dir}/build)
    execute_process(
        COMMAND rm -rf ${_dir}
    )

endif()

set(GLider_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GLider/lib/cmake/GLider" PARENT_SCOPE)
