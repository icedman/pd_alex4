cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})


if (NOT ${ENVSDK} STREQUAL "")
	# Convert path from Windows
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
	execute_process(
			COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
			COMMAND head -n 1
			COMMAND cut -c9-
			OUTPUT_VARIABLE SDK
			OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if (NOT EXISTS ${SDK})
	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
	return()
endif()

if ($ENV{BUILD_DEVICE})
	include(${SDK}/C_API/buildsupport/arm.cmake)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

# Game Name Customization
set(PLAYDATE_GAME_NAME alex4)
set(PLAYDATE_GAME_DEVICE alex4_DEVICE)

project(${PLAYDATE_GAME_NAME} C ASM)

file(GLOB SRC_FILES 
	engine/platform.c
	engine/render.c
	engine/camera.c
	engine/utils.c
	engine/animation.c
	engine/image.c
	engine/input.c
	engine/trace.c
	engine/entity.c
	engine/engine.c
	engine/noise.c
	engine/font.c
	engine/json.c
	engine/map.c
	engine/sound.c
	alex/main.h
	alex/main.c
	alex/alex4.h
	alex/actor.c
	alex/actor.h
	alex/bullet.c
	alex/bullet.h
	alex/control.c
	alex/control.h
	alex/fixed_math.c
	alex/fixed_math.h
	alex/data.c
	alex/data.h
	alex/hisc.c
	alex/hisc.h
	alex/map.c
	alex/map.h
	# alex/options.c
	# alex/options.h
	alex/particle.c
	alex/particle.h
	alex/player.c
	alex/player.h
	alex/script.c
	alex/script.h
	alex/scroller.c
	alex/scroller.h
	alex/timer.c
	alex/timer.h
	alex/token.c
	alex/token.h
	alex/misc.c
	alex/misc.h
	ext/iniparser/dictionary.h
	ext/iniparser/dictionary.c
	ext/iniparser/iniparser.h
	ext/iniparser/iniparser.c
	ext/miniz/miniz.h
	ext/miniz/miniz.c
	# new ports need to implement these
	alex/sound.h
	alex/port.h
	src/game_main.c
	src/port.c
	src/options.c
	src/sound.c
	ext/iniparser/dictionary.h
	ext/iniparser/dictionary.c
	ext/iniparser/iniparser.h
	ext/iniparser/iniparser.c
	ext/miniz/miniz.h
	ext/miniz/miniz.c
)

if (TOOLCHAIN STREQUAL "armgcc")
	add_executable(${PLAYDATE_GAME_DEVICE}
	${SRC_FILES}
	)
	target_include_directories(${PLAYDATE_GAME_DEVICE} 
		PRIVATE
		./
		src
		engine
		alex
		ext/iniparser
		ext/miniz
	)
	target_compile_definitions(${PLAYDATE_GAME_DEVICE} PUBLIC PLATFORM_PLAYDATE=1 RENDER_PLAYDATE=1 HAS_RAND_SUPPORT MINIZ_NO_ZLIB_COMPATIBLE_NAMES MINIZ_NO_ZLIB_APIS MINIZ_NO_ARCHIVE_WRITING_APIS MINIZ_NO_TIME _LARGEFILE64_SOURCE)
else()
	add_library(${PLAYDATE_GAME_NAME} SHARED
	${SRC_FILES}
	)
	target_include_directories(${PLAYDATE_GAME_NAME} 
		PRIVATE
		./
		src
		engine
		alex
		ext/iniparser
		ext/miniz
	)
	target_compile_definitions(${PLAYDATE_GAME_NAME} PUBLIC PLATFORM_PLAYDATE=1 RENDER_PLAYDATE=1 HAS_RAND_SUPPORT MINIZ_NO_ZLIB_COMPATIBLE_NAMES MINIZ_NO_ZLIB_APIS MINIZ_NO_ARCHIVE_WRITING_APIS MINIZ_NO_TIME _LARGEFILE64_SOURCE)
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)

