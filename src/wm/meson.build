wm_includes = ['public']

wm_sources = [
                'private/general/shapes.cpp',
                'private/xlib/flow_wm_xlib.cpp',
                'private/xlib/client/client.cpp',
                'private/xlib/client_manager/client_manager.cpp',
                'private/xlib/screens/screens.cpp',
                'private/xlib/handlers/handler_helpers.cpp',
                'private/xlib/handlers/handlers.cpp',
                'private/xlib/keyboard_manager/keyboard_manager.cpp',
                'private/general/config.cpp',
                'private/general/keybinding.cpp',
                'private/xlib/cursor/cursor.cpp'
]

wm_deps = [dependency('X11'), dependency('xrandr'), dependency('nlohmann_json'), dependency('xft'), dependency('fontconfig')]

flow_wm_lib = shared_library(
    'flow_wm_lib',
     wm_sources,
     include_directories : wm_includes,
     install: true,
     dependencies: wm_deps
)