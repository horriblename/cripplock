gtk = dependency('gtk+-3.0')
executable('demo',
  'main.cpp',
  dependencies: [gtk],
  # option deosn't show up in compilation_commands.json if I don't set this?
  override_options : ['cpp_std=c++20'],
)

subdir('test2')
