project('MyApp', 'cpp')

cxx = meson.get_compiler('cpp')
openssl = dependency('openssl', required: true)

executable('myapp', '../myapp.cpp', dependencies:  openssl )
