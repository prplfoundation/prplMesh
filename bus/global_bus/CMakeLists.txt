add_executable(global_bus ${src} global_bus.cpp)
target_link_libraries(global_bus ${libs})
install(TARGETS global_bus DESTINATION bin)
install(FILES global_bus.conf DESTINATION share)
