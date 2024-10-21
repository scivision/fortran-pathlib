set(_noenv)

if(HAVE_Fortran_FILESYSTEM)

foreach(t IN ITEMS env setenv)
  add_executable(test_fortran_${t} test_${t}.f90)
  target_link_libraries(test_fortran_${t} PRIVATE ffilesystem)
  target_compile_options(test_fortran_${t} PRIVATE ${ffilesystem_fortran_test_flags})
  set_property(TARGET test_fortran_${t} PROPERTY LINKER_LANGUAGE Fortran)
endforeach()

add_test(NAME Fortran_${t} COMMAND test_fortran_setenv)

add_test(NAME FortranHomedirEnv COMMAND test_fortran_env)
add_test(NAME FortranHomedirNoEnv COMMAND test_fortran_env)

list(APPEND _noenv FortranHomedirNoEnv)

set_property(TEST FortranHomedirEnv FortranHomedirNoEnv PROPERTY LABELS Fortran)

endif()


add_executable(test_cpp_env test_env.cpp)
target_link_libraries(test_cpp_env PRIVATE ffilesystem)
target_include_directories(test_cpp_env PRIVATE ..)

add_test(NAME CppHomedirEnv COMMAND test_cpp_env)
add_test(NAME CppHomedirNoEnv COMMAND test_cpp_env)

list(APPEND _noenv CppHomedirNoEnv)

set_property(TEST CppHomedirEnv CppHomedirNoEnv PROPERTY LABELS Cpp)


if(CMAKE_VERSION VERSION_LESS 3.22 OR "$ENV{CI}")
  set_property(TEST ${_noenv} PROPERTY DISABLED true)
else()
  set_property(TEST ${_noenv} PROPERTY ENVIRONMENT_MODIFICATION "HOME=unset:;USERPROFILE=unset:;TEMP=unset:;TMPDIR=unset:")
endif()
