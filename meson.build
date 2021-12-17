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

subdir('src')
subproject('shell-base')

run_command('scripts/move_config.sh')
