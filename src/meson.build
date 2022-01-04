includes = []
main_dep = []

wm_version = '0.0.1'
wm_options = '-DFLOW_BETTER_FOCUS'
add_global_arguments(['-DFLOW_WM_VERSION="'+wm_version+'"', wm_options], language: 'cpp')

subdir('external')
subdir('logger')
subdir('wm')


exe = executable(
    'main',
    'main.cpp',
    include_directories: includes,
    link_with: [flow_wm_lib, flow_logger_lib],
    dependencies: main_dep,
    install: true,
)


test('basic', exe)