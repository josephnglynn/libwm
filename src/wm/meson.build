wm_includes = [include_directories('public')]

wm_sources = [
                'private/general/shapes.cpp',
                'private/xlib/flow_wm_xlib.cpp',
                'private/xlib/client/client.cpp',
                'private/xlib/client_manager/client_manager.cpp',
                'private/xlib/screens/screens.cpp',
                'private/xlib/handlers/handlers.cpp',
                'private/xlib/keyboard_manager/keyboard_manager.cpp',
                'private/general/config.cpp',
                'private/general/keybinding.cpp',
                'private/xlib/cursor/cursor.cpp',
                'private/general/input_functions.cpp',
                'private/xlib/shell/shell.cpp',
]

wm_deps = [dependency('x11'), dependency('xinerama'), dependency('nlohmann_json'), dependency('xft'), dependency('fontconfig'), dependency('threads'), meson.get_compiler('cpp').find_library('dl')]

flow_wm_lib = shared_library(
    'flow_wm_lib',
     wm_sources,
     include_directories : external_includes + wm_includes,
     install: true,
     link_with: [shell_base],
     dependencies: wm_deps
)
