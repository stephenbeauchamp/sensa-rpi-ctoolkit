# ------------------------------------------------
# Generic Makefile
#
# Author: yanick.rochon@gmail.com
# Date  : 2011-08-10
#
# Changelog :
#   2010-11-05 - first version
#   2011-08-10 - added structure : sources, objects, binaries
#                thanks to http://stackoverflow.com/users/128940/beta
#   2017-04-24 - changed order of linker params
# ------------------------------------------------

# project name (generate executable with this name)
TARGET   = ctoolkit_main
CONFIG   = $(TARGET).config

CC       = gcc
# compiling flags here
CFLAGS   = -std=c99 -Wall -I. -I/usr/include/hiredis -lhiredis

LINKER   = gcc
# linking flags here
LFLAGS   = -Wall -I. -lm

# change these to proper directories where each file should be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin
TSTDIR   = tst

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f
cp       = cp -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@$(cp) ${TSTDIR}/config.txt ${BINDIR}/${CONFIG}
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@$(rm) ${BINDIR}/${CONFIG}
	@echo "Executable removed!"

.PHONY: watch
watch:
	while true; do \
		if ! make -q ${WATCH_TARGET}; then \
			echo "\n\n\n ===== `date +'%y%m%d %H%M%S'` ====="; \
			make ${WATCH_TARGET}; \
		fi; \
		sleep 3; \
	done
