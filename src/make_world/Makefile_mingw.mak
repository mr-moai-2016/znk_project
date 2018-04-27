MAKE_CMD=mingw32-make
MKID=mingw

# Entry rule.
all:
	call _make_kick.bat $(MAKE_CMD) $(MKID)

# Install rule.
install:
	call _make_kick.bat $(MAKE_CMD) $(MKID) install

# Clean rule.
clean:
	call _make_kick.bat $(MAKE_CMD) $(MKID) clean
