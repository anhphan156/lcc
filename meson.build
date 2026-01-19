project('lcc', 'c')

incdir = include_directories('include')

exe = executable('lcc', 
  'src/main.c',
  'src/lexer.c',
  include_directories : incdir,
)

run_target('run', 
  command: [
    exe,
    '../test_input/00'
  ]
)
