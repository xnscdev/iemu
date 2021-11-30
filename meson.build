project('iemu', 'c', version: '0.0.1', license: 'GPL-3.0-or-later',
        default_options: ['c_std=gnu11'])

if host_machine.endian() != 'little'
  error('IEMU only supports little-endian host machines.')
endif

epoxy_dep = dependency('epoxy', required: true)
glfw_dep = dependency('glfw3', required: true)

iemu_src = [
  'exc.c',
  'inst.c',
  'main.c',
  'opcode.c',
  'register.c',
  'task.c'
]

executable('iemu', iemu_src, dependencies: [epoxy_dep, glfw_dep], install: true)
