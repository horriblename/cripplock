gtkmm = dependency('gtkmm-3.0')
executable('withmm',
  'main.cpp',
  dependencies: [gtkmm],
  # option deosn't show up in compilation_commands.json if I don't set this?
  override_options : ['cpp_std=c++20'],
)
