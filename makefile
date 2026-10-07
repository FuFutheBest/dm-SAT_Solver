COURSESATBUILD=/home/fufu/Dev/dm/LAB1-SAT_Solver/build
all:
	$(MAKE) -C "$(COURSESATBUILD)"
clean:
	@if [ -d "$(COURSESATBUILD)" ]; \
	then \
	  if [ -f "$(COURSESATBUILD)"/makefile ]; \
	  then \
	     touch "$(COURSESATBUILD)"/build.hpp; \
	     $(MAKE) -C "$(COURSESATBUILD)" clean; \
	  fi; \
	  rm -rf "$(COURSESATBUILD)"; \
	fi
	rm -f "/home/fufu/Dev/dm/LAB1-SAT_Solver/src/makefile"
	rm -f "/home/fufu/Dev/dm/LAB1-SAT_Solver/makefile"
test:
	$(MAKE) -C "$(COURSESATBUILD)" test
part1_test:
	$(MAKE) -C "$(COURSESATBUILD)" part1_test
part2_test:
	$(MAKE) -C "$(COURSESATBUILD)" part2_test

public_test:
	$(MAKE) -C "$(COURSESATBUILD)" public_test

handin:
	$(MAKE) -C "$(COURSESATBUILD)" handin
coursesat:
	$(MAKE) -C "$(COURSESATBUILD)" coursesat
format:
	$(MAKE) -C "$(COURSESATBUILD)" format
.PHONY: all coursesat clean test format part1_test part2_test public_test handin
