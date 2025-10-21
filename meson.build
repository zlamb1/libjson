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
    'src/json_error.c',
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

n_tests = [
    'empty',
    'bad_int_hyphen',
    'bad_int_non_digit',
    'bad_frac',
    'bad_exp',
    'bad_exp_hyphen',
    'bad_exp_plus',
    'ws.json',
]

n_ext_tests = [
    'trailing_decimal',
]

y_tests = [
    [ 'num_int',        '1.000000'      ],
    [ 'num_frac',       '1.435610'      ],
    [ 'num_exp',        '14.356100'     ],
    [ 'num_int_exp',    '100000.000000' ],
    [ 'num_exp_hyphen', '0.110000'      ],
    [ 'num_exp_plus',   '131.230000'    ],
    [ 'ws',             '50.000000'     ],
]

y_ext_tests = [
    ['trailing_decimal', '1.000000'],
]

foreach test : n_tests
    test(f'n_@test@', tester, args : [f'@test_dir@/n/@test@.json'], should_fail : true)
endforeach

foreach test : n_ext_tests
    test(f'n_ext_@test@', tester, args : [f'@test_dir@/n/ext_@test@.json', '-e'], should_fail : true)
endforeach

foreach test : y_tests
    test_name = test[0]
    args = [f'@test_dir@/y/@test_name@.json']

    if test.length() > 1
        args += test[1]
    endif

    test(f'y_@test_name@', tester, args : args)
endforeach

foreach test : y_ext_tests
    test_name = test[0]
    args = [f'@test_dir@/y/ext_@test_name@.json']

    if test.length() > 1
        args += test[1]
    endif

    args += '-e'

    test(f'y_@test_name@', tester, args : args)
endforeach