env = Environment()
env['CXX']='clang++'
env.Append(CXXFLAGS=['-O2', '-g', '-std=c++11', '-Wall', '-Wextra', '-pedantic'])

libperfmon = env.SharedLibrary(
    target='perfmon',
    source=[
        'cpu_frequency.cpp',
        'perfmon.cpp'
    ]
)
