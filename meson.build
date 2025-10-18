project('Better Control', 'c',
        default_options: [ 'c_std=c23', 'warning_level=3' ],
        meson_version: '>=1.8.0',
        version: '0.0.1',
        license: 'GPL-3.0-or-later',
        license_files: [ 'COPYING' ])

add_project_dependencies([
    dependency('gtk+-3.0'),
], language: 'c')

git_hash = run_command('git', 'rev-parse', 'HEAD',
                       check: true).stdout().strip()

app_git_hash_define = '-DAPP_GIT_HASH="@0@"'.format(git_hash)

add_project_arguments([
    '-DAPP_VERSION="@0@"'.format(meson.project_version()),
    '-DAPP_GIT_HASH="@0@"'.format(git_hash),
    '-DAPP_NAME="@0@"'.format(meson.project_name()),
    '-DAPP_ID="org.BetterDE.Better-Control"',
], language: 'c')

gnome = import('gnome')

resources = gnome.compile_resources('resource',
                                    'gresources.xml',
                                    source_dir: '.',)

subdir('src')
executable('better-control', [ source_files, resources ],
           include_directories: include_directories('include'),
           install: true)