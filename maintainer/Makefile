all:
	@echo pretty      - indent code
	@echo upload_docs - upload documentation
	@echo coverage    - create coverage report

pretty:
	astyle --style=allman --indent=spaces=4 --indent-modifiers \
	   --indent-switches --indent-preproc-block --indent-preproc-define \
	   --indent-col1-comments --pad-oper --pad-header --align-pointer=type \
	   --align-reference=type --add-brackets --convert-tabs --close-templates \
	   --lineend=linux --preserve-date --suffix=none --formatted --recursive \
	   "include/crow/*.hpp,cpp" "tests/*.cpp,hpp"

upload_docs:
	mkdir build ; cd build ; cmake .. ; cmake --build . --target crow_documentation ; cd ..
	cd build/doc ; ../../doc/scripts/git-update-ghpages nlohmann/crow html
	open http://nlohmann.github.io/crow/

coverage:
	mkdir build ; cd build ; cmake .. -DCROW_CHECK_COVERAGE=ON ; cmake --build . ; cd ..
	cd build ; ./tests ; cd ..
	cd build ; cmake --build . --target crow_coverage_report ; cd ..
	open build/coverage_report/index.html
