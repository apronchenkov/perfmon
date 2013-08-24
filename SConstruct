env = Environment()
if env['PLATFORM'] == 'darwin':
   env['CXX']='clang++'
   env.Append(CXXFLAGS=['-O2', '-g', '-std=c++11', '-Wall', '-Wextra', '-pedantic', '-pthread', '-stdlib=libc++'])
   env.Append(LINKFLAGS=['-stdlib=libc++'])
else:
   env['CXX']='g++'
   env.Append(CXXFLAGS=['-O2', '-g', '-std=c++0x', '-Wall', '-Wextra', '-pedantic', '-pthread'])

libperfmon = env.SharedLibrary(
    target='perfmon',
    source=[
        'cpu_frequency.cpp',
        'perfmon.cpp'
    ]
)

test = env.Program(['test.cpp', 'test_scope.cpp', 'test_statement.cpp'], LIBS=[libperfmon, 'boost_unit_test_framework'])
