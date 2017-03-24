CXX=g++
CFLAGS=-std=c++1z -pedantic -W -Wall -Wcast-qual -Wconversion -Werror -Wextra -Wwrite-strings
IFLAGS=
LFLAGS=
OFLAGS=

SOURCES=test/slice.cpp test/ranger.cpp
OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))
DEPENDENCIES=$(OBJECTS:.o=.d)

# COMMANDS
test: test/slice
	./test/slice

clean:
	$(RM) $(DEPENDENCIES) $(OBJECTS) test/ranger test/slice

# INFERENCES
%.o: %.cpp
	$(CXX) $(CFLAGS) $(OFLAGS) $(IFLAGS) -MMD -MP -c $< -o $@

# DEPENDENCIES
#test/catch.hpp:
#	curl https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp > test/catch.hpp

test/ranger: $(OBJECTS)
test/slice: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LFLAGS) $(OFLAGS) -o $@

-include $(DEPENDENCIES)
