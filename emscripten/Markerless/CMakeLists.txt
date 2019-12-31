set(PUBLIC_HEADERS
    include/ARPipeline.hpp
    include/CameraCalibration.hpp
    include/DebugHelpers.hpp
    include/GeometryTypes.hpp
    include/Pattern.hpp
    include/PatternDetector.hpp
)

set(SOURCE
    src/ARPipeline.cpp
    src/CameraCalibration.cpp
    src/GeometryTypes.cpp
    src/Pattern.cpp
    src/PatternDetector.cpp
)

add_library(MARKERLESS STATIC
    ${PUBLIC_HEADERS} ${SOURCE}
)

target_include_directories(MARKERLESS
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${JPEG_INCLUDE_DIR}
    PRIVATE ${OpenCV_INCLUDE_DIR}
)

target_link_libraries(MARKERLESS
    INTERFACE ${LIBS}
    PRIVATE ${JPEG_LIBRARIES}
)

if(NOT ARX_TARGET_PLATFORM_WINDOWS)
	target_link_libraries(MARKERLESS
		PRIVATE ${OpenCV_LIBS}
	)
else()
	set (LINK_LIST "")
	foreach(x ${OpenCV_LIBS})
		string(REPLACE ".lib" "341d.lib" x_debug ${x})
		list (APPEND LINK_LIST debug ${x_debug} optimized ${x})
	endforeach()
	target_link_libraries(MARKERLESS
	    PUBLIC ${LINK_LIST}
)
  endif()
