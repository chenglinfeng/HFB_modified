# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

# srouces
aux_source_directory(. SRC)

IF(module_build_shared)

    set(module_share_name ${module_name}_shared)

    add_library(${module_share_name} SHARED ${SRC} ../dectree/decision_tree_test.cpp)
    SET_TARGET_PROPERTIES(${module_share_name} PROPERTIES OUTPUT_NAME ${module_name})
    SET_TARGET_PROPERTIES(${module_share_name} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

    # Link Microsoft SEAL
    target_link_libraries(${module_share_name}  SEAL::seal)

    install(TARGETS ${module_share_name}
        DESTINATION lib
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

ENDIF(module_build_shared)


IF(module_build_static)

    set(module_static_name ${module_name}_static)

    add_library(${module_static_name} STATIC ${SRC})
    SET_TARGET_PROPERTIES(${module_static_name} PROPERTIES OUTPUT_NAME ${module_name})
    SET_TARGET_PROPERTIES(${module_static_name} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

    # Link Microsoft SEAL
    target_link_libraries(${module_static_name}  SEAL::seal)

    install(TARGETS ${module_static_name}
        DESTINATION lib
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

ENDIF(module_build_static)
