project('lcc', 'c')

if get_option('buildtype') == 'debug'
  add_project_arguments('-DDEBUG_BUILD', language: 'c')
endif

lcc = executable(
  'lcc', 
  'src/main.c',
  'src/lexer.c',
  'src/symbol_table.c',
  'src/codegen/codegen.c',
  'src/codegen/x86/cg.c',
  'src/codegen/asm_file.c',
  'src/parser/parser.c',
  'src/parser/expression.c',
  'src/parser/statement.c',
  'src/ast/ast_visualizer.c',
  'src/ast/prepass.c',
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
  ['test4', 'input_04', 'expected_04'],
  ['test5', 'input_05', 'expected_05'],
  ['test6', 'input_06', 'expected_06'],
  ['test7', 'input_07', 'expected_07'],
  ['test8', 'input_08', 'expected_08'],
  ['test9', 'input_09', 'expected_09'],
  ['test10', 'input_10', 'expected_10'],
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
