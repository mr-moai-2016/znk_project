# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)/../mkfsys
endif

GCC_CMD=gcc
PLATFORM=win32
ifeq ($(MACHINE), x64)
  PLATFORM=win64
else
  MACHINE=x86
  PLATFORM=win32
endif
# Output directory
ABINAME=mingw$(MACHINE)$(DEBUG)
O = ./out_dir/$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -g 
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)_mingwd
SLIBS_DIR=slib/$(PLATFORM)_mingwd
else
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG 
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)
SLIBS_DIR=slib/$(PLATFORM)
endif

CP=cp

INCLUDE_FLAG+=  \
	-I$(MY_LIBS_ROOT)/libRano \
	-I$(MY_LIBS_ROOT)/libZnk \


include Makefile_version.mak

BASENAME0=patgen
EXE_FILE0=$O/patgen.exe
OBJS0=\
	$O/pat_diff.o \
	$O/pat_make.o \
	$O/main.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/libtls-17.dll \



# Entry rule.
all: $O $(EXE_FILE0) 

# Mkdir rule.
$O:
	mkdir -p $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) 
	@echo $(LINKER) -o $(EXE_FILE0)  {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.a $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.a -lws2_32 
	@     $(LINKER) -o $(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.a $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.a -lws2_32 


##
# Pattern rule.
# We use not suffix rule but pattern rule for dealing flexibly with files in sub-directory.
# In this case, there is very confusing specification, that is :
# '\' to the left hand of ':' works as escape sequence, 
# '\' to the right hand of ':' does not work as escape sequence. 
# Hence, we have to duplicate '\' to the left hand of ':',
# the other way, '\' to the right hand of ':' we have to put only single '\',
# for example $O\\%.o: $S\%.c .
#
$O/%.o: $S/%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O/%.o: $S/%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	mkdir -p ../../patgen 
	mkdir -p ../../patgen/apply_this_scripts/android 
	mkdir -p ../../patgen/apply_this_scripts/cygwin 
	mkdir -p ../../patgen/apply_this_scripts/linux 
	mkdir -p ../../patgen/apply_this_scripts/windows 
	for tgt in cert.pem ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/ ; fi ; done
	for tgt in patgen.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/ ; fi ; done
	for tgt in pat_make.bat ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/ ; fi ; done
	for tgt in pat_make.sh ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/ ; fi ; done
	for tgt in pat_diff.bat ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/ ; fi ; done
	for tgt in pat_diff.sh ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/ ; fi ; done
	for tgt in apply_this_scripts/android/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/apply_this_scripts/android/ ; fi ; done
	for tgt in apply_this_scripts/cygwin/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/apply_this_scripts/cygwin/ ; fi ; done
	for tgt in apply_this_scripts/linux/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/apply_this_scripts/linux/ ; fi ; done
	for tgt in apply_this_scripts/windows/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/apply_this_scripts/windows/ ; fi ; done

# Install exec rule.
install_exec: $(EXE_FILE0)
	mkdir -p ../../patgen/$(PLATFORM) 
	for tgt in $(EXE_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/$(PLATFORM)/ ; fi ; done
	@for tgt in $(RUNTIME_FILES) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../patgen/$(PLATFORM)/ ; fi ; done

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	rm -rf $O/ 

# Src and Headers Dependency
main.o: pat_diff.h pat_make.h
pat_diff.o: pat_diff.h
