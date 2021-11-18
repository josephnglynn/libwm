wm_includes = ['public']
wm_sources = ['private/x11/flow_wm.cpp']
wm_deps = [dependency('X11')]

flow_wm_lib = shared_library(
    'flow_wm_lib',
     wm_sources,
     include_directories : wm_includes,
     install: true,
     dependencies: wm_deps
)