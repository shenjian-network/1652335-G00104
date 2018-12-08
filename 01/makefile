CC=g++
Target = sender1
.PHONY: all clean 
HEADFILE= ../common/common.h sdl.h snl.h SPL.h
INCLUDEFILE= ../common/common.cpp sdl.cpp snl.cpp SPL.cpp 
all:$(Target) 
$(Target):%:%.cpp $(HEADFILE) $(INCLUDEFILE)
	$(CC) -o $@ $< $(HEADFILE) $(INCLUDEFILE)
clean:
	-rm -rf $(Target)
