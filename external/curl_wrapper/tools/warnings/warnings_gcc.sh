#!/bin/bash

rm -fr build_warnings
mkdir build_warnings
cd build_warnings

# We apply the following warning suppressions:
#
# -Wno-abi-tag                            we do not care about ABI tags
# -Wno-aggregate-return                   returning structs is fine
# -Wno-missing-declarations               this would make the code too verbose
# -Wno-multiple-inheritance               this is desired for the Hive (Bumblebee + Health Checker)
# -Wno-effc++                             these warnings do not improve code
# -Wno-long-long                          long longs are allowed in Modern C++
# -Wno-namespaces                         namespaces are fine
# -Wno-padded                             warnings about padding can be ignored
# -Wno-switch-enum                        we chose switch-default as enum style
# -Wno-system-headers                     do not warn about system headers
# -Wno-templates                          templates are fine
# -Wno-shadow                             shadowed variable names are acceptable
# -Wno-sign-conversion                    some sign conversions are desired
# -Wno-suggest-override                   GCC does a bad job with final
# -Wno-useless-cast                       useless casts may be unavoidable in thirdparty code
#
# We further ignore:
#
# - all warnings regarding C++ standard   we do not care about past and future language versions
# - all warnings with a fixed size        we do not have such fixed limits

CXXFLAGS="-W \
-Wabi \
-Wno-abi-tag \
-Waddress \
-Wno-aggregate-return \
-Waggressive-loop-optimizations \
-Waliasing \
-Walign-commons \
-Waligned-new=all \
-Wall \
-Walloc-zero \
-Walloca \
-Wampersand \
-Wargument-mismatch \
-Warray-bounds \
-Warray-bounds=2 \
-Warray-temporaries \
-Wassign-intercept \
-Wattribute-alias \
-Wattributes \
-Wbad-function-cast \
-Wbool-compare \
-Wbool-operation \
-Wbuiltin-declaration-mismatch \
-Wbuiltin-macro-redefined \
-Wc-binding-type \
-Wcast-align \
-Wcast-align=strict \
-Wcast-function-type \
-Wcast-qual \
-Wcatch-value \
-Wcatch-value=3 \
-Wchar-subscripts \
-Wcharacter-truncation \
-Wchkp \
-Wclass-memaccess \
-Wclobbered \
-Wcomment \
-Wcomments \
-Wcompare-reals \
-Wconditionally-supported \
-Wconversion \
-Wconversion-extra \
-Wconversion-null \
-Wcoverage-mismatch \
-Wcpp \
-Wctor-dtor-privacy \
-Wdangling-else \
-Wdate-time \
-Wdeclaration-after-statement \
-Wdelete-incomplete \
-Wdelete-non-virtual-dtor \
-Wdeprecated \
-Wdeprecated-declarations \
-Wdesignated-init \
-Wdisabled-optimization \
-Wdiscarded-array-qualifiers \
-Wdiscarded-qualifiers \
-Wdiv-by-zero \
-Wdo-subscript \
-Wdouble-promotion \
-Wduplicate-decl-specifier \
-Wduplicated-branches \
-Wduplicated-cond \
-Wno-effc++ \
-Wempty-body \
-Wendif-labels \
-Wenum-compare \
-Werror-implicit-function-declaration \
-Wexpansion-to-defined \
-Wextra \
-Wextra-semi \
-Wfloat-conversion \
-Wfloat-equal \
-Wformat \
-Wformat-contains-nul \
-Wformat-extra-args \
-Wformat-nonliteral \
-Wformat-overflow=2 \
-Wformat-security \
-Wformat-signedness \
-Wformat-truncation \
-Wformat-truncation=2 \
-Wformat-y2k \
-Wformat-zero-length \
-Wformat=2 \
-Wframe-address \
-Wfree-nonheap-object \
-Wfunction-elimination \
-Whsa \
-Wif-not-aligned \
-Wignored-attributes \
-Wignored-qualifiers \
-Wimplicit \
-Wimplicit-fallthrough \
-Wimplicit-fallthrough=5 \
-Wimplicit-function-declaration \
-Wimplicit-int \
-Wimplicit-interface \
-Wimplicit-procedure \
-Wincompatible-pointer-types \
-Winherited-variadic-ctor \
-Winit-self \
-Winline \
-Wint-conversion \
-Wint-in-bool-context \
-Wint-to-pointer-cast \
-Winteger-division \
-Wintrinsic-shadow \
-Wintrinsics-std \
-Winvalid-memory-model \
-Winvalid-offsetof \
-Winvalid-pch \
-Wjump-misses-init \
-Wline-truncation \
-Wliteral-suffix \
-Wlogical-not-parentheses \
-Wlogical-op \
-Wno-long-long \
-Wlto-type-mismatch \
-Wmain \
-Wmaybe-uninitialized \
-Wmemset-elt-size \
-Wmemset-transposed-args \
-Wmisleading-indentation \
-Wmissing-attributes \
-Wmissing-braces \
-Wno-missing-declarations \
-Wmissing-field-initializers \
-Wmissing-format-attribute \
-Wmissing-include-dirs \
-Wmissing-noreturn \
-Wmissing-parameter-type \
-Wmissing-prototypes \
-Wmultichar \
-Wno-multiple-inheritance \
-Wmultistatement-macros \
-Wno-namespaces \
-Wnarrowing \
-Wnested-externs \
-Wnoexcept \
-Wnoexcept-type \
-Wnon-template-friend \
-Wnon-virtual-dtor \
-Wnonnull \
-Wnonnull-compare \
-Wnormalized \
-Wnull-dereference \
-Wodr \
-Wold-style-cast \
-Wold-style-declaration \
-Wold-style-definition \
-Wopenmp-simd \
-Woverflow \
-Woverlength-strings \
-Woverloaded-virtual \
-Woverride-init \
-Woverride-init-side-effects \
-Wpacked \
-Wpacked-bitfield-compat \
-Wpacked-not-aligned \
-Wno-padded \
-Wparentheses \
-Wpedantic \
-Wplacement-new \
-Wplacement-new=2 \
-Wpmf-conversions \
-Wpointer-arith \
-Wpointer-compare \
-Wpointer-sign \
-Wpointer-to-int-cast \
-Wpragmas \
-Wproperty-assign-default \
-Wprotocol \
-Wpsabi \
-Wreal-q-constant \
-Wrealloc-lhs \
-Wrealloc-lhs-all \
-Wredundant-decls \
-Wregister \
-Wreorder \
-Wrestrict \
-Wreturn-local-addr \
-Wreturn-type \
-Wscalar-storage-order \
-Wselector \
-Wsequence-point \
-Wno-shadow \
-Wshadow-compatible-local \
-Wshadow-ivar \
-Wshadow-local \
-Wshadow=compatible-local \
-Wshadow=global \
-Wshadow=local \
-Wshift-count-negative \
-Wshift-count-overflow \
-Wshift-negative-value \
-Wshift-overflow \
-Wshift-overflow=2 \
-Wsign-compare \
-Wno-sign-conversion \
-Wsign-promo \
-Wsized-deallocation \
-Wsizeof-array-argument \
-Wsizeof-pointer-div \
-Wsizeof-pointer-memaccess \
-Wstack-protector \
-Wstrict-aliasing \
-Wstrict-aliasing=3 \
-Wstrict-null-sentinel \
-Wstrict-overflow \
-Wstrict-overflow=5 \
-Wstrict-prototypes \
-Wstrict-selector-match \
-Wstringop-overflow \
-Wstringop-overflow=4 \
-Wstringop-truncation \
-Wsubobject-linkage \
-Wsuggest-attribute=cold \
-Wsuggest-attribute=const \
-Wsuggest-attribute=format \
-Wsuggest-attribute=malloc \
-Wsuggest-attribute=noreturn \
-Wsuggest-attribute=pure \
-Wsuggest-final-methods \
-Wsuggest-final-types \
-Wno-suggest-override \
-Wsurprising \
-Wswitch \
-Wswitch-bool \
-Wswitch-default \
-Wno-switch-enum \
-Wswitch-unreachable \
-Wsync-nand \
-Wsynth \
-Wno-system-headers \
-Wtabs \
-Wtarget-lifetime \
-Wtautological-compare \
-Wno-templates \
-Wterminate \
-Wtraditional \
-Wtraditional-conversion \
-Wtrampolines \
-Wtrigraphs \
-Wtype-limits \
-Wundeclared-selector \
-Wundef \
-Wundefined-do-loop \
-Wunderflow \
-Wuninitialized \
-Wunknown-pragmas \
-Wunreachable-code \
-Wunsafe-loop-optimizations \
-Wunsuffixed-float-constants \
-Wunused \
-Wunused-but-set-parameter \
-Wunused-but-set-variable \
-Wno-unused-const-variable \
-Wunused-dummy-argument \
-Wunused-function \
-Wunused-label \
-Wunused-local-typedefs \
-Wunused-macros \
-Wunused-parameter \
-Wunused-result \
-Wunused-value \
-Wunused-variable \
-Wuse-without-only \
-Wno-useless-cast \
-Wvarargs \
-Wvariadic-macros \
-Wvector-operation-performance \
-Wvirtual-inheritance \
-Wvirtual-move-assign \
-Wvla \
-Wvolatile-register-var \
-Wwrite-strings \
-Wzero-as-null-pointer-constant \
-Wzerotrip \
-frequire-return-statement" \
CXX=g++ \
CC=gcc \
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
