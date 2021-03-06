import os
import glob
from copy import copy

Import ('env')

test_env = env.Clone()

test_env['LIBS'] = [env['MAPNIK_NAME']]
test_env.AppendUnique(LIBS=copy(env['LIBMAPNIK_LIBS']))
if env['RUNTIME_LINK'] == 'static' and env['PLATFORM'] == 'Linux':
    test_env.AppendUnique(LIBS='dl')
    test_env.AppendUnique(LIBS='rt')
test_env.AppendUnique(CXXFLAGS='-g')
test_env['CXXFLAGS'] = copy(test_env['LIBMAPNIK_CXXFLAGS'])
test_env.Append(CPPDEFINES = env['LIBMAPNIK_DEFINES'])
if test_env['HAS_CAIRO']:
    test_env.PrependUnique(CPPPATH=test_env['CAIRO_CPPPATHS'])
    test_env.Append(CPPDEFINES = '-DHAVE_CAIRO')
if env['PLATFORM'] == 'Darwin':
    test_env.Append(LINKFLAGS='-F/ -framework CoreFoundation')

test_env_local = test_env.Clone()

for cpp_test in glob.glob('*cpp'):
    test_program = test_env_local.Program('out/'+cpp_test.replace('.cpp',''), source=[cpp_test])
    if 'install' in COMMAND_LINE_TARGETS:
        env.Alias('install',test_program)
    #Depends(test_program, env.subst('../src/%s' % env['MAPNIK_LIB_NAME']))
