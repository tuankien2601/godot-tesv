#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/", "extern/gli/", "extern/gli/external/", "extern/lz4/", "extern/nifly/"])
sources = []
for root, dirs, files in os.walk('src'):
    scons_path = os.path.join(root, "*.cpp")
    sources.extend(Glob(scons_path))
    
for root, dirs, files in os.walk('extern/nifly'):
    cpp_path = os.path.join(root, "*.cpp")
    sources.extend(Glob(cpp_path))
    
for root, dirs, files in os.walk('extern/lz4'):
    c_path = os.path.join(root, "*.c")
    sources.extend(Glob(c_path))

if env["platform"] == "macos":
    library = env.SharedLibrary(
        "demo/bin/libtesv.{}.{}.framework/libtesv.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            "demo/bin/libtesv.{}.{}.simulator.a".format(env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            "demo/bin/libtesv.{}.{}.a".format(env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        "demo/bin/libtesv{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
env.Tool('compilation_db')

# Add the CompilationDatabase builder as a target
# This will create compile_commands.json in the current directory by default
env.CompilationDatabase()