link_directories(${CMAKE_BINARY_DIR}/lib)

# SET(module_dectree ${module_name}_dectree)

# aux_source_directory(. DECTREE_SRC)

# add_executable(${module_dectree} ${DECTREE_SRC})

add_executable(decision_tree_test decision_tree_test.cpp ot_one.cpp ot_two.cpp ot_three.cpp common/dectree.cpp ../src/strategytb.cpp ../src/sealtable.cpp)

# target_link_libraries(${module_dectree} ${module_dectree} SEAL::seal)
# find_package(SEAL 3.3.0 EXACT REQUIRED)

target_link_libraries(decision_tree_test ${module_name} SEAL::seal)