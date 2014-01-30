SHELL := /bin/bash

RCXX=$(shell root-config --cflags)
RLIB=$(shell root-config --libs)

CXX		= g++
CFLAGS		= -pipe -g \
-D __SKIP_CLASS_DEF__ \
-I.. -I../plotFake \
-Wall -Wextra -Winit-self -Wcast-align -Wno-comment -Wno-ignored-qualifiers \
-O2 -march=native -mfpmath=sse -fPIC 

#---- Paths ----#
ELS_ana = ../ELS_ana
src = ../src

#---- Links ----#
links = -I$(ELS_ana)

CXXFLAGS	= $(CFLAGS) $(RCXX) -lTreePlayer
LDFLAGS		= -lm -L. $(RLIB)

#---- Packages Dependencies ----#
TREE_DEPS	=	\
$(ELS_ana)/MakeTree.h	\
$(ELS_ana)/MyTree.h 

READ_DEPS = \
$(ELS_ana)/MyTree.h \
$(ELS_ana)/ReadBack.h

Targets	= makeTree readBack

all: Makefile $(Targets)

obj/%.so: $(src)/%.cxx
	$(CXX) -c $< -o $@ $(SOFLAGS) $(CXXFLAGS) $(links)

#---- My Trees ----#
obj/MyTree_Dict.cxx: $(ELS_ana)/MyTree.h $(ELS_ana)/LinkDef.h
	rootcint -f $@ -c -I$(ELS_ana) $+
obj/MyTree_Dict.so: obj/MyTree_Dict.cxx $(TREE_DEPS)
	$(CXX) -c $< -o $@ $(SOFLAGS) $(CXXFLAGS) -Os -g1 -I$(ELS_ana)

#---- Additional Objects ----#
#obj/MyTree.so: $(TREE_DEPS) $(ELS_ana)/LinkDef.h
obj/MakeTree.so:    $(TREE_DEPS)
obj/ReadBack.so:    $(READ_DEPS)

#---- Analysis Code ----#
makeTree:                \
	obj/MakeTree.so  \
	obj/MyTree_Dict.so	
	$(CXX) $+ -o $@ $(CXXFLAGS) $(LDFLAGS) $(link)

readBack:                \
	obj/ReadBack.so  \
	obj/MyTree_Dict.so	
	$(CXX) $+ -o $@ $(CXXFLAGS) $(LDFLAGS) $(link)

clean:
	rm -f *.run
	rm -f obj/*.so
	rm -f obj/*.so
	rm -f obj/*.cxx
	rm -f obj/*.h
