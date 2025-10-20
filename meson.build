project(
    'libjson', 
    'c',
    meson_version : '>= 1.3.0',
    version : '0.1',
    default_options : ['warning_level=3', 'c_std=c99', 'werror=true'],
)

srcs = [
    'src/json_array.c',
    'src/json_decode.c',
    'src/json_object.c',
]

lib = library(
  'json',
  srcs,
  include_directories : 'include',
  install : true,
)