# Script made by [SG-10]Cpt.Moore
# Note that this will convert line endings in source files to LF.

SOURCEDIR=src
PROJECTDIR=zombiereloaded
SMINCLUDES=include
BUILDDIR=build
SPCOMP_LINUX=bin/linux/spcomp-1.5.3
SPCOMP_DARWIN=bin/darwin/spcomp-1.5.3
DOS2UNIX_LINUX=dos2unix -p
DOS2UNIX_DARWIN=bin/darwin/dos2unix -p
#VERSIONDUMP=./updateversion.sh

OS = $(shell uname -s)
ifeq "$(OS)" "Darwin"
	SPCOMP = $(SPCOMP_DARWIN)
	DOS2UNIX = $(DOS2UNIX_DARWIN)
else
	SPCOMP = $(SPCOMP_LINUX)
	DOS2UNIX = $(DOS2UNIX_LINUX)
endif

vpath %.sp $(SOURCEDIR)
vpath %.sp $(SOURCEDIR)/$(PROJECTDIR)
vpath %.smx $(BUILDDIR)

SOURCEFILES=$(SOURCEDIR)/$(PROJECTDIR)/*.sp
OBJECTS=$(patsubst %.sp, %.smx, $(notdir $(wildcard $(SOURCEFILES))))

all: clean prepare $(OBJECTS)

prepare: prepare_newlines prepare_builddir

prepare_newlines:
	@echo "Removing windows newlines"
	@find $(SMINCLUDES) -name \*.inc -exec $(DOS2UNIX) '{}' \;
	@find $(SOURCEDIR) -name \*.inc -exec $(DOS2UNIX) '{}' \;
	@find $(SOURCEDIR) -name \*.sp -exec $(DOS2UNIX) '{}' \;

prepare_builddir:
	@echo "Creating build directory"
	@mkdir -p $(BUILDDIR)

%.smx: %.sp
	#$(VERSIONDUMP)
	$(SPCOMP) -i$(SOURCEDIR) -i$(SMINCLUDES) -o$(BUILDDIR)/$@ $<

clean:
	@echo "Removing build directory"
	@rm -fr $(BUILDDIR)
