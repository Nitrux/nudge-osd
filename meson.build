project('nudgeosd', 'cpp',
  version: '0.1.0',
  default_options: ['cpp_std=c++17', 'warning_level=2']
)

# Add x86-64-v3 optimizations for x86_64 architecture
cpp_args = []
if host_machine.cpu_family() == 'x86_64'
  cpp_args += ['-march=x86-64-v3']
endif

# Dependencies
qt6 = import('qt6')
dep_qt6 = dependency('qt6', version: '>=6.9', modules: ['Core', 'Gui', 'Qml', 'Quick', 'QuickControls2', 'DBus'])
dep_maui = dependency('MauiKit4', version: '>=4.0.5')
dep_kf6coreaddons = dependency('KF6CoreAddons', version: '>=6.13.0')
cpp_compiler = meson.get_compiler('cpp')
dep_kf6i18n = declare_dependency(
    include_directories: include_directories(get_option('prefix') / 'include' / 'KF6' / 'KI18n'),
    dependencies: [cpp_compiler.find_library('KF6I18n'), cpp_compiler.find_library('KF6I18nQml')],
)

dep_layer_shell = cpp_compiler.find_library('LayerShellQtInterface', required: false)
if not dep_layer_shell.found()
  error('LayerShellQt not found. Please install liblayershellqtinterface-dev')
endif

# Process QML resources and MOC files
qt6_res = qt6.preprocess(
  qresources: 'resources.qrc',
  moc_headers: 'src/Controller.h'
)

# Sources
sources = [
  'src/main.cpp',
  'src/Controller.cpp',
  qt6_res
]

# Executable
executable('nudge-osd',
  sources,
  dependencies: [dep_qt6, dep_maui, dep_layer_shell, dep_kf6coreaddons, dep_kf6i18n],
  cpp_args: cpp_args,
  install: true
)
