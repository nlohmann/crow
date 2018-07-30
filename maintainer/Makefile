all:
	@echo pretty      - indent code
	@echo upload_docs - upload documentation

pretty:
	astyle --style=allman --indent=spaces=4 --indent-modifiers \
	   --indent-switches --indent-preproc-block --indent-preproc-define \
	   --indent-col1-comments --pad-oper --pad-header --align-pointer=type \
	   --align-reference=type --add-brackets --convert-tabs --close-templates \
	   --lineend=linux --preserve-date --suffix=none --formatted \
	   include/crow/crow.hpp tests/unittests.cpp

upload_docs:
	mkdir build ; cd build ; cmake .. ; cmake --build . --target doc ; cd ..
	cd build/doc ; ../../doc/scripts/git-update-ghpages nlohmann/crow html
	rm -fr build
	open http://nlohmann.github.io/crow/
