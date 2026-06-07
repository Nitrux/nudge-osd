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
dep_maui = dependency('MauiKit4', version: '>=4.0.4')
dep_kf6coreaddons = dependency('KF6CoreAddons', version: '>=6.13.0')

# Try multiple names for LayerShellQt
dep_layer_shell = dependency('LayerShellQt', required: false)
if not dep_layer_shell.found()
  dep_layer_shell = dependency('LayerShellQtInterface', required: false)
endif
if not dep_layer_shell.found()
  # Fallback: try to find the library directly
  dep_layer_shell = meson.get_compiler('cpp').find_library('LayerShellQtInterface', required: false)
endif
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
  dependencies: [dep_qt6, dep_maui, dep_layer_shell, dep_kf6coreaddons],
  cpp_args: cpp_args,
  install: true
)
