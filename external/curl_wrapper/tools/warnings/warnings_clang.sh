#!/bin/bash

rm -fr build_warnings
mkdir build_warnings
cd build_warnings

CXXFLAGS="-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-covered-switch-default -Wno-exit-time-destructors -Wno-global-constructors -Wno-padded -Wno-missing-prototypes -Wno-switch-enum -Wno-weak-vtables -fno-caret-diagnostics -fno-diagnostics-fixit-info -Wno-disabled-macro-expansion -O0" \
CFLAGS="-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-covered-switch-default -Wno-exit-time-destructors -Wno-global-constructors -Wno-padded -Wno-missing-prototypes -Wno-switch-enum -Wno-weak-vtables -fno-caret-diagnostics -fno-diagnostics-fixit-info -Wno-disabled-macro-expansion -O0" \
CXX=clang++ \
CC=clang \
cmake .. -GNinja

echo "Compiling..."

time ninja | grep "\[\-W" | grep "^../include/http" | sed 's|../||' > warnings.log

# abort if Ninja failed
if [[ "${PIPESTATUS[0]}" != "0" ]]; then
    echo "Compilation failed!"
    ninja
    exit 1
fi

mv warnings.log ..

cd ..

cat warnings.log | sort | uniq > warnings_sorted.log
mv warnings_sorted.log warnings.log

cat warnings.log
test ! -s warnings.log
