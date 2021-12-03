logger_includes = include_directories('public')
logger_sources = ['private/logger.cpp']
logger_deps = []

flow_logger_lib = shared_library(
    'flow_logger_lib',
     logger_sources,
     include_directories : logger_includes,
     install: true,
     dependencies: logger_deps
)
