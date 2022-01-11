includes = []
main_dep = []


subdir('wm')

includes += external_includes

exe = executable(
    'main',
    'main.cpp',
    include_directories: includes,
    link_with: [flow_wm_lib, logger],
    dependencies: main_dep,
    install: true,
)


test('basic', exe)