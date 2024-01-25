if(BUILD_TESTING)
  enable_testing()
endif()

add_subdirectory(hashmap)
add_subdirectory(test)

install(TARGETS
  hashmaplib
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)

install(
  FILES 
    ${CMAKE_SOURCE_DIR}/project/hashmap/include/hashmap.h 
  DESTINATION 
    include/hashmap)
