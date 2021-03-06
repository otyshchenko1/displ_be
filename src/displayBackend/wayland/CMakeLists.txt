set(SOURCES
	Compositor.cpp
	Connector.cpp
	Display.cpp
	SharedBuffer.cpp
	SharedFile.cpp
	SharedMemory.cpp
	Shell.cpp
	ShellSurface.cpp
	Surface.cpp
)

if(WITH_DRM)
	list(APPEND SOURCES
		DrmBuffer.cpp
		WaylandDrm.cpp
		wayland-drm/wayland-drm-protocol.c
	)
endif()

if(WITH_IVI_EXTENSION)
	list(APPEND SOURCES
		IlmControl.cpp
		IviApplication.cpp
		IviSurface.cpp
	)
endif()

if(WITH_INPUT)
	list(APPEND SOURCES
		Seat.cpp
		SeatKeyboard.cpp
		SeatPointer.cpp
		SeatTouch.cpp
	)
endif()

add_library(display_wayland STATIC ${SOURCES})

target_link_libraries(display_wayland ${WAYLAND_LIBRARIES})

if(WITH_IVI_EXTENSION)
	target_link_libraries(display_wayland
		ilmCommon
		ilmClient
		ilmControl
	)
endif()
