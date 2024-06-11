# Author: Tomas Brablec
# Project: pacppman (ICP project)
# License: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)
#
builddir = build
appname = pacppman

.PHONY: build run clean doxygen pack

build: 
	@mkdir -p $(builddir) && cd $(builddir) && cmake .. && make

run: build
	./$(builddir)/$(appname)

clean: 
	@rm -rf doc build xbrabl04

doxygen:
	@doxygen

pack: clean
	zip -r xbrabl04.zip *
