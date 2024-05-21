add_library( 
  myutils  
  utils.c
)
message("Add myutils.")

target_include_directories(myutils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")