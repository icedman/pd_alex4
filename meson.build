project('test_data', 'c')

pwd = meson.current_source_dir()
cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : true, disabler: true)
gl_dep = dependency('opengl', required: true, disabler: true)
sdl2_dep = dependency('SDL2', required: true, disabler: true)
sdl2_mixer_dep = dependency('SDL2_Mixer', required: true, disabler: true)
sdl2_image_dep = dependency('SDL2_image', required: true, disabler: true)

alex_files = [
	'alex/main.h',
	'alex/main.c',
	'alex/alex4.h',
	'alex/actor.c',
	'alex/actor.h',
	'alex/bullet.c',
	'alex/bullet.h',
	'alex/control.c',
	'alex/control.h',
	'alex/fixed_math.c',
	'alex/fixed_math.h',
	'alex/data.c',
	'alex/data.h',
	'alex/hisc.c',
	'alex/hisc.h',
	'alex/map.c',
	'alex/map.h',
	# 'alex/options.c',
	# 'alex/options.h',
	'alex/particle.c',
	'alex/particle.h',
	'alex/player.c',
	'alex/player.h',
	'alex/script.c',
	'alex/script.h',
	'alex/scroller.c',
	'alex/scroller.h',
	'alex/timer.c',
	'alex/timer.h',
	'alex/token.c',
	'alex/token.h',
	'alex/misc.c',
	'alex/misc.h',
	# new ports need to implement these
	'alex/sound.h',
	'alex/port.h',
	'alex/options.h',
	'alex/options.c',
	'ext/iniparser/dictionary.h',
	'ext/iniparser/dictionary.c',
	'ext/iniparser/iniparser.h',
	'ext/iniparser/iniparser.c',
	'ext/miniz/miniz.h',
	'ext/miniz/miniz.c'
]

engine_files = [
        'engine/platform.c',
        'engine/render.c',
        'engine/camera.c',
        'engine/utils.c',
        'engine/animation.c',
        'engine/image.c',
        'engine/input.c',
        'engine/trace.c',
        'engine/entity.c',
        'engine/engine.c',
        'engine/noise.c',
        'engine/font.c',
        'engine/json.c',
        'engine/sound.c',
        'engine/map.c',
]

game_files = [
    'src/game_main.c',
    'src/port.c',
    'src/options.c',
    'src/sound.c',
]

includes = include_directories(
    'ext/iniparser',
    'ext/miniz',
    'engine',
    'alex',
    'src'
)

# executable('test_data', 
# 	'test.c',
# 	game_files,
# 	include_directories: includes,
# 	dependencies: [ m_dep ]
# )

# if get_option('b_sanitize') == 'address'
    # add_project_arguments('-fsanitize=address', language: 'c')
    # add_project_link_arguments('-fsanitize=address', language: 'c')
# endif

executable('game',
    engine_files,
    alex_files,
    game_files,
    include_directories: [ includes ],
    c_args: [
    	'-DRENDER_SOFTWARE', '-DPLATFORM_SDL', '-Wno-unused-parameter', '-Wno-unused-variable', '-DHAS_RAND_SUPPORT',
    	'-DMINIZ_NO_ZLIB_COMPATIBLE_NAMES',
    	'-DMINIZ_NO_ZLIB_APIS',
    	'-DMINIZ_NO_ARCHIVE_WRITING_APIS',
    	'-DMINIZ_NO_TIME',
    	'-D_LARGEFILE64_SOURCE',
    ],
    dependencies: [ m_dep, sdl2_dep, sdl2_mixer_dep, sdl2_image_dep, gl_dep ]
)
