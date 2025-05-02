# Create a library called libpdb which includes its source files
add_library (pdb
  pdb.h
  # KHSdefs.h
  structs/PDBnode_t.h
  # structs/KHSbacknode_t.h
  # structs/KHSlabelednode_t.h
  # structs/KHSbucket_t.h
  # structs/KHSsolution_t.h
  # structs/KHSksolution_t.h
  # structs/KHSIBaseNode.h
  # algorithm/KHSbsolver.h
  # algorithm/KHSmA.h
  # algorithm/KHSbela.h
  # algorithm/KHSIK.h
  # algorithm/KHSIKStar.h
  )

# Generate the main header file from its template
configure_file(pdb.h.in ${CMAKE_CURRENT_SOURCE_DIR}/pdb.h)

set_target_properties(pdb PROPERTIES LINKER_LANGUAGE CXX)

# Make sure the compiler can find include files for the library when other
# libraries or executables link to it
target_include_directories (pdb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
