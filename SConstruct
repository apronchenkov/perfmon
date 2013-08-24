env = Environment()
if env['PLATFORM'] == 'darwin':
   env['CXX']='clang++'
   env.Append(CXXFLAGS=['-O2', '-g', '-std=c++11', '-Wall', '-Wextra', '-pedantic', '-stdlib=libc++'])
   env.Append(LINKFLAGS=['-stdlib=libc++', '-L/usr/local/lib/x86_64', '-ljit', "-lperfmon"])
else:
   env['CXX']='g++'
   env.Append(CXXFLAGS=['-O2', '-g', '-std=c++0x', '-Wall', '-Wextra', '-pedantic', '-pthread'])
   env.Append(LINKFLAGS=['-pthread'])

libperfmon = env.SharedLibrary(
    target='perfmon',
    source=[
        'cpu_frequency.cpp',
        'perfmon.cpp'
    ]
)
