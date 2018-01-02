CXX = g++
CXXFLAGS = -c -Wall -pedantic -O2 -std=c++17
BINARY = linda-client

SOURCES = \
		src/StringOrNumber.cpp \
		src/Tuple.cpp \
		src/main.cpp \
		src/Buffer.cpp \
		src/QueryLexer.cpp \
		src/QueryParser.cpp \

OBJECTS=$(SOURCES:.cpp=.o)

$(BINARY): $(OBJECTS)
	$(CXX) $^ -o $@ $(LINK_FLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	@rm -f $(BINARY)
	@rm -f src/*.o src/*.gch

.PHONY:
	clean
