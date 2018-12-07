CC=g++
Target = sender1
.PHONY: all clean 
HEADFILE= common.h sdl.h snl.h
INCLUDEFILE= common.cpp sdl.cpp snl.cpp 
all:$(Target) 
$(Target):%:%.cpp $(HEADFILE) $(INCLUDEFILE)
	$(CC) -o $@ $< $(HEADFILE) $(INCLUDEFILE)
clean:
	-rm -rf $(Target) txt/* txt
create:
	mkdir txt
