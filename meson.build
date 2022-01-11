project('wm', 'cpp',
  version : '0.0.1',
  default_options : [
          'warning_level=3',
          'cpp_std=c++17'
  ]
)

if get_option('buildtype').startswith('debug')
    add_project_arguments('-DDEBUG', language: 'cpp')
endif

wm_version = '0.0.1'
wm_options = '-DFLOW_BETTER_FOCUS'
add_global_arguments(['-DFLOW_WM_VERSION="'+wm_version+'"', wm_options], language: 'cpp')

subdir('external')
subdir('src')

run_command('scripts/move_config.sh')
