project('lcc', 'c')

if get_option('buildtype') == 'debug'
  add_project_arguments('-DDEBUG_BUILD', language: 'c')
endif

exe = executable(
  'lcc', 
  'src/main.c',
  'src/lexer.c',
  'src/parser.c',
  'src/codegen.c',
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
    exe,
    run_input
  ]
)
