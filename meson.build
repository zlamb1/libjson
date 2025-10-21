project(
    'libjson', 
    'c',
    meson_version : '>= 1.3.0',
    version : '0.1',
    default_options : ['warning_level=3', 'c_std=c99', 'werror=true'],
)

srcs = [
    'src/json_array.c',
    'src/json_bool.c',
    'src/json_decoder.c',
    'src/json_number.c',
    'src/json_object.c',
    'src/json_string.c',
    'src/json_value.c'
]

cc = meson.get_compiler('c')
m_dep = cc.find_library('m')

lib = library(
  'json',
  srcs,
  dependencies : [m_dep],
  include_directories : 'include',
  install : true,
)

tester = executable(
    'tester',
    'tests/tester.c',
    include_directories : 'include',
    install : true,
    link_with : lib
)

test_dir = meson.project_source_root() + '/tests'

fail_tests = [
    'top_level_empty',
    'top_level_bad_int_hyphen',
    'top_level_bad_int_non_digit',
    'top_level_bad_frac',
    'top_level_bad_exp',
    'top_level_bad_exp_hyphen',
    'top_level_bad_exp_plus',
]

pass_tests = [
    ['top_level_num_int',        '1.000000'     ],
    ['top_level_num_frac',       '1.435610'     ],
    ['top_level_num_exp',        '14.356100'    ],
    ['top_level_num_int_exp',    '100000.000000'],
    ['top_level_num_exp_hyphen', '0.110000'],
    ['top_level_num_exp_plus', '131.230000'],
]

foreach test : fail_tests
    test(f'test_fail_@test@', tester, args : [f'@test_dir@/fail/@test@.json'], should_fail : true)
endforeach

foreach test : pass_tests
    test_name = test[0]
    stdout = test[1]
    test(f'test_pass_@test_name@', tester, args : [f'@test_dir@/pass/@test_name@.json', stdout])
endforeach