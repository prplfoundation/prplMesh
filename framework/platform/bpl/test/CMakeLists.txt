add_executable(bpl_test bpl_test.cpp)
target_link_libraries(bpl_test dl bpl common elpp)
target_include_directories(bpl_test PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../common/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
	)
