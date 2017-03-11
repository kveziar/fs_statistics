set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(FSSTAT_TARGET_NAME ${PROJECT_NAME})

set(FSSTAT_SOURCES
	file_system_scanner.cpp
	file_system_scanner.h
	formatters.cpp
	formatters.h
	MainWindowModel.cpp
	MainWindowModel.h
	StatisticsModel.cpp
	StatisticsModel.h
	main.cpp
	qml.qrc
)

source_group("Source Files" FILES ${FSSTAT_SOURCES})

find_package(Qt5 COMPONENTS Core Quick Widgets REQUIRED)

add_executable(${FSSTAT_TARGET_NAME}
	${FSSTAT_SOURCES}
)

target_link_libraries(${FSSTAT_TARGET_NAME}
	Qt5::Core
	Qt5::Quick
	Qt5::Widgets
)

if(WIN32)
	if(MSVC)
		# http://stackoverflow.com/a/11785733
		set_target_properties(${FSSTAT_TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE "/subsystem:windows /entry:mainCRTStartup")
	endif(MSVC)
endif(WIN32)
