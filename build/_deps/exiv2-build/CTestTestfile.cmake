# CMake generated Testfile for 
# Source directory: /home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src
# Build directory: /home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(bugfixTests "cmake" "-E" "env" "EXIV2_BINDIR=/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/bin" "/usr/bin/python3" "runner.py" "--verbose" "bugfixes")
set_tests_properties(bugfixTests PROPERTIES  WORKING_DIRECTORY "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/tests" _BACKTRACE_TRIPLES "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/CMakeLists.txt;124;add_test;/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/CMakeLists.txt;0;")
add_test(lensTests "cmake" "-E" "env" "EXIV2_BINDIR=/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/bin" "/usr/bin/python3" "runner.py" "--verbose" "lens_tests")
set_tests_properties(lensTests PROPERTIES  WORKING_DIRECTORY "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/tests" _BACKTRACE_TRIPLES "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/CMakeLists.txt;127;add_test;/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/CMakeLists.txt;0;")
add_test(tiffTests "cmake" "-E" "env" "EXIV2_BINDIR=/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/bin" "/usr/bin/python3" "runner.py" "--verbose" "tiff_test")
set_tests_properties(tiffTests PROPERTIES  WORKING_DIRECTORY "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/tests" _BACKTRACE_TRIPLES "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/CMakeLists.txt;130;add_test;/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/CMakeLists.txt;0;")
add_test(versionTests "cmake" "-E" "env" "EXIV2_BINDIR=/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/bin" "/usr/bin/python3" "runner.py" "--verbose" "bash_tests/version_test.py")
set_tests_properties(versionTests PROPERTIES  WORKING_DIRECTORY "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/tests" _BACKTRACE_TRIPLES "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/CMakeLists.txt;133;add_test;/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/CMakeLists.txt;0;")
add_test(regressionTests "cmake" "-E" "env" "EXIV2_BINDIR=/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/bin" "/usr/bin/python3" "runner.py" "--verbose" "regression_tests")
set_tests_properties(regressionTests PROPERTIES  WORKING_DIRECTORY "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/tests" _BACKTRACE_TRIPLES "/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/CMakeLists.txt;136;add_test;/home/keepersam02/coding_projects/02-personal/batch_photo_processor/build/_deps/exiv2-src/CMakeLists.txt;0;")
subdirs("xmpsdk")
subdirs("src")
subdirs("app")
