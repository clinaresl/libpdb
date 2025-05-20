# Create a library called libpdb which includes its source files
add_library (pdb SHARED
  pdb.h
  PDBdefs.h
  gui/PDBprogress_t.h
  structs/PDBnode_t.h
  structs/PDBopen_t.h
  structs/PDBpdb_t.h
  algorithm/PDBpdb.h
  algorithm/PDBoutpdb.h
  algorithm/PDBinpdb.h
  )

target_include_directories(pdb
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Install the library binary
install(TARGETS pdb
        EXPORT pdb_targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include)

# generate the header from the template and provide an install directive
configure_file(pdb.h.in ${CMAKE_CURRENT_SOURCE_DIR}/pdb.h)
install(FILES pdb.h DESTINATION include)

# Optionally export targets for CMake find_package
install(EXPORT pdb_targets
    FILE pdbConfig.cmake
    NAMESPACE pdb::
    DESTINATION lib/cmake/pdb
)

set_target_properties(pdb PROPERTIES LINKER_LANGUAGE CXX)

# Make sure the compiler can find include files for the library when other
# libraries or executables link to it
# target_include_directories (pdb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
