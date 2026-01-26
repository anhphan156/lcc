project('lcc', 'c')

if get_option('buildtype') == 'debug'
  add_project_arguments('-DDEBUG_BUILD', language: 'c')
endif

exe = executable(
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

run_input = '../test_input/00'

run_target(
  'run', 
  command: [
    exe,
    run_input
  ]
)

run_target(
  'valgrind',
  command: [
    'valgrind',
    '--leak-check=full',
    '-s',
    exe,
    run_input
  ]
)
