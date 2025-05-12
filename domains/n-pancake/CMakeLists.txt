add_executable (npancake
  getopt1.c
  getopt.c
  ../helpers.cc
  npancake_t.cc
  npancake.cc)

add_executable (query
  getopt1.c
  getopt.c
  ../helpers.cc
  query.cc)

target_link_libraries (npancake LINK_PUBLIC pdb)
