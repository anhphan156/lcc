project('lcc', 'c')

exe = executable(
  'lcc', 
  'src/main.c',
  'src/lexer.c',
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
