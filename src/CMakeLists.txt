# Create a library called libpdb which includes its source files
add_library (pdb
  pdb.h
  PDBdefs.h
  structs/PDBnode_t.h
  structs/PDBopen_t.h
  structs/PDBpdb_t.h
  algorithm/PDBpdb.h
  algorithm/PDBoutpdb.h
  algorithm/PDBinpdb.h
  )

# Generate the main header file from its template
configure_file(pdb.h.in ${CMAKE_CURRENT_SOURCE_DIR}/pdb.h)

set_target_properties(pdb PROPERTIES LINKER_LANGUAGE CXX)

# Make sure the compiler can find include files for the library when other
# libraries or executables link to it
target_include_directories (pdb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
