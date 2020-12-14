# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

link_directories(${CMAKE_BINARY_DIR}/lib)

SET(module_test ${module_name}_test)

# srouces
aux_source_directory(. TEST_SRC)

add_executable(${module_test} ${TEST_SRC})

target_link_libraries(${module_test} ${module_name} SEAL::seal libmorseot.so)
