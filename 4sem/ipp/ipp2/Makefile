##
# IPP Project Task 2 Makefile
# Author: Zbynek Krivka, v2024-02-04
#

LOGIN=xbrabl04
TEMP_DIR=temp
TASK=2
SCRIPT=interpret.php

PHONY := all clean testall test

all: check
	cd test && ./test.sh
	cd test2 && ./test-int.py

stan:
	vendor/phpstan/phpstan/phpstan analyse --level 9

pack: student/*
	cd student && zip -r $(LOGIN).zip  * -x __MACOSX/* .git/* && mv $(LOGIN).zip ../

check: pack vendor
	./is_it_ok.sh $(LOGIN).zip $(TEMP_DIR) $(TASK) 

run-help: interpret.php
	if [ "${HOSTNAME}" = "merlin.fit.vutbr.cz" ]; then php8.3 $(SCRIPT) --help; else php $(SCRIPT) --help; fi

vendor: composer.phar
	if [ "${HOSTNAME}" = "merlin.fit.vutbr.cz" ]; then php8.3 $< install; else php $< install; fi

clean:
	$(RM) *.zip is_it_ok.log
	$(RM) -r $(TEMP_DIR) tmp
