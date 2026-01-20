project('lcc', 'c')

exe = executable(
  'lcc', 
  'src/main.c',
  'src/lexer.c',
  include_directories: 'include',
  install: true,
)

run_target(
  'run', 
  command: [
    exe,
    '../test_input/00'
  ]
)

run_target(
  'valgrind',
  command: [
    'valgrind',
    '--leak-check=full',
    exe,
    '../test_input/00'
  ]
)
