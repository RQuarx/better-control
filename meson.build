project(
    'Better Control',
    'c',
    default_options: ['c_std=c23', 'warning_level=3'],
    meson_version: '>=1.8.0',
    version: '0.0.1',
    license: 'GPL-3.0-or-later',
    license_files: ['COPYING'],
)

deps = [dependency('gtk+-3.0')]

if get_option('sound-module')
    deps += dependency('libpipewire-0.3')
endif

if get_option('network-module')
    deps += dependency('libnm')
endif

args = [
    '-DAPP_VERSION="' + meson.project_version() + '"',
    '-DAPP_NAME="' + meson.project_name() + '"',
    '-DAPP_ID="org.BetterDE.Better-Control"',
    '-DGTK_DISABLE_DEPRECATED',
    '-DGDK_DISABLE_DEPRECATED',
    '-xc',
]

if get_option('colored-log')
    args += '-DCOLORED_LOG_MESSAGE'
endif

gnome = import('gnome')

resource_data = gnome.compile_resources(
    'resources',
    'resources.gresource.xml',
    source_dir: '.',
    dependencies: [],
    export: true,
)

subdir('src')
executable(
    'better-control',
    [source_files, resource_data],
    include_directories: include_directories('include'),
    install: true,
    c_args: args,
    dependencies: deps,
)