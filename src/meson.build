includes = []
main_dep = []

subdir('wm')

exe = executable(
    'main',
    'main.cpp',
    include_directories: includes,
    link_with: flow_wm_lib,
    dependencies: main_dep,
    install: true,
)


test('basic', exe)