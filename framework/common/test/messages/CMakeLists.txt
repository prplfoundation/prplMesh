add_library(dummy_messages dummy1.cpp dummy2.cpp)
target_link_libraries(dummy_messages PRIVATE common)
install(TARGETS dummy_messages DESTINATION ${CMAKE_INSTALL_LIBDIR})
