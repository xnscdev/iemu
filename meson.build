project('iemu', 'c', version: '0.0.1', license: 'GPL-3.0-or-later')

epoxy_dep = dependency('epoxy', required: true)
glfw_dep = dependency('glfw3', required: true)

iemu_src = [
  'main.c',
  'register.c',
  'task.c'
]

executable('iemu', iemu_src, dependencies: [epoxy_dep, glfw_dep], install: true)
