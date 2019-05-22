file(GLOB sources *.cpp)
foreach(src ${sources})
	get_filename_component(target ${src} NAME_WE)
	add_executable(${target}_CPP ${src})
	target_link_libraries(${target}_CPP ${libs})
	install(TARGETS ${target}_CPP DESTINATION bin)
endforeach(src ${sources})

