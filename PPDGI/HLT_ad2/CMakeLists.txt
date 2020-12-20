link_directories(${CMAKE_BINARY_DIR}/lib)

# SET(module_dectree ${module_name}_dectree)

# aux_source_directory(. DECTREE_SRC)

# add_executable(${module_dectree} ${DECTREE_SRC})

add_executable(HLT_ad2 HLT_ad2.cpp ../test/ot_one.cpp ../test/ot_two.cpp ../test/ot_three.cpp ../dectree/common/dectree.cpp ../src/strategytb.cpp ../src/sealtable.cpp)

# target_link_libraries(${module_dectree} ${module_dectree} SEAL::seal)
# find_package(SEAL 3.3.0 EXACT REQUIRED)

target_link_libraries(HLT_ad2 ${module_name} SEAL::seal)