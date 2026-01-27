project('lcc', 'c')

if get_option('buildtype') == 'debug'
  add_project_arguments('-DDEBUG_BUILD', language: 'c')
endif

lcc = executable(
  'lcc', 
  'src/main.c',
  'src/lexer.c',
  'src/codegen.c',
  'src/symbol_table.c',
  'src/parser/parser.c',
  'src/parser/expression.c',
  'src/parser/statement.c',
  'src/ast/ast_visualizer.c',
  'src/ast/ast.c',
  include_directories: 'include',
  install: true,
)

lc_code = '../tests/input_00'

generated_binary = custom_target(
  'generated_exe',
  output: 'out',
  command: [
    lcc,
    lc_code,
    '@OUTPUT@'
  ]
)

foreach t : [
  ['test1', 'input_01', 'expected_01'],
  ['test2', 'input_02', 'expected_02'],
  ['test3', 'input_03', 'expected_03'],
]
test(
  t[0],
  find_program('./tests/compare.sh'),
  args: [
    lcc,
    meson.current_build_dir(),
    files('./tests/' + t[1]),
    files('./tests/' + t[2]),
  ],
  is_parallel: false
)
endforeach

run_target(
  'run', 
  command: [
    lcc,
    lc_code
  ]
)

run_target(
  'manual_test',
  command: [generated_binary],
)

run_target(
  'valgrind',
  command: [
    'valgrind',
    '--leak-check=full',
    '-s',
    lcc,
    lc_code
  ]
)
