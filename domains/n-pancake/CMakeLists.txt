add_executable (npancake
  getopt1.c
  getopt.c
  npancake_t.cc
  npancake.cc)

target_link_libraries (npancake LINK_PUBLIC pdb)
