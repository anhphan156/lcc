project('lcc', 'c')

if get_option('buildtype') == 'debug'
  add_project_arguments('-DDEBUG_BUILD', language: 'c')
endif

lcc = executable(
  'lcc', 
  'src/main.c',
  'src/lexer.c',
  'src/codegen.c',
  'src/parser/parser.c',
  'src/parser/expression.c',
  'src/parser/statement.c',
  'src/ast/ast_visualizer.c',
  'src/ast/ast.c',
  include_directories: 'include',
  install: true,
)

lc_code = '../test_input/00'

generated_binary = custom_target(
  'generated_exe',
  output: 'out',
  command: [
    lcc,
    lc_code,
    '@OUTPUT@'
  ]
)


run_target(
  'run', 
  command: [
    lcc,
    lc_code
  ]
)

run_target(
  'run_test',
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
