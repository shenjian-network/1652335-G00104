CC=g++
Target1 = sender2
Target2 = receiver2
.PHONY: all clean 
HEADFILE1= common.h sdl.h snl.h SPL.h mainProc.h PL.h dl.h nl.h
INCLUDEFILE1= common.cpp sdl.cpp snl.cpp SPL.cpp mainProc.cpp PL.cpp dl.cpp nl.cpp
HEADFILE2= common.h mainProc.h PL.h RPL.h rdl.h rnl.h dl.h nl.h
INCLUDEFILE2= common.cpp mainProc.cpp PL.cpp RPL.cpp rdl.cpp rnl.cpp dl.cpp nl.cpp
all:$(Target1) $(Target2)
$(Target1):%:%.cpp $(HEADFILE1) $(INCLUDEFILE1)
	$(CC) -o $@ $< $(HEADFILE1) $(INCLUDEFILE1)
$(Target2):%:%.cpp $(HEADFILE2) $(INCLUDEFILE2)
	$(CC) -o $@ $< $(HEADFILE2) $(INCLUDEFILE2)
clean:
	-rm -rf $(Target1) $(Target2) *.share.* output
