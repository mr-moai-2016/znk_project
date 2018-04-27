# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)/../mkfsys
endif

ifndef MACHINE
  UNAME_M=$(shell uname -m)
  ifeq ($(UNAME_M), i386)
    MACHINE=x86
  endif
  ifeq ($(UNAME_M), i686)
    MACHINE=x86
  endif
  ifeq ($(UNAME_M), x86_64)
    MACHINE=x64
  endif
  ifeq ($(UNAME_M), amd64)
    MACHINE=x64
  endif
endif
GCC_CMD=gcc
PLATFORM=linux
ifeq ($(MACHINE), x64)
  GCC_CMD=gcc -m64
  PLATFORM=linux64
endif
ifeq ($(MACHINE), x86)
  GCC_CMD=gcc -m32
  PLATFORM=linux32
endif
# Output directory
ABINAME = linux$(MACHINE)$(DEBUG)
O = ./out_dir/$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -g
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)d
SLIBS_DIR=slib/$(PLATFORM)d
else
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)
SLIBS_DIR=slib/$(PLATFORM)
endif

CP=cp

INCLUDE_FLAG+=  \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \


include Makefile_version.mak

BASENAME0=custom_boy
EXE_FILE0=$O/custom_boy.cgi
OBJS0=\
	$O/CB_config.o \
	$O/CB_custom_automatic.o \
	$O/CB_custom_cookie.o \
	$O/CB_custom_feature.o \
	$O/CB_custom_postvars.o \
	$O/CB_fgp_info.o \
	$O/CB_finger.o \
	$O/CB_snippet.o \
	$O/CB_ua_conf.o \
	$O/CB_ua_info.o \
	$O/CB_vars_base.o \
	$O/CB_virtualizer.o \
	$O/CB_wgt_prim.o \
	$O/CB_wgt_prim_db.o \
	$O/main.o \

SUB_LIBS=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/libZnk.so \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/libRano.so \



# Entry rule.
all: $O $(EXE_FILE0) 

# Mkdir rule.
$O:
	mkdir -p $O


# Product files rule.
$(EXE_FILE0): $(OBJS0)
	$(LINKER) -o $(EXE_FILE0) $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so -lpthread -ldl -lstdc++ 


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
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c -fPIC $<

$O/%.o: $S/%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c -fPIC $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/templates 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/publicbox 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/RE 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/UA 
	for tgt in custom_boy.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/ ; fi ; done
	for tgt in templates/*.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/templates/ ; fi ; done
	for tgt in publicbox/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/publicbox/ ; fi ; done
	for tgt in RE/*.png ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/RE/ ; fi ; done
	for tgt in RE/*.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/RE/ ; fi ; done
	for tgt in UA/*.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/UA/ ; fi ; done

# Install exec rule.
install_exec: $(EXE_FILE0)
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy 
	for tgt in $(EXE_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/ ; fi ; done

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	rm -r $O/ 

# Src and Headers Dependency
CB_config.o: CB_config.h
CB_custom_automatic.o: CB_custom_automatic.h
CB_custom_cookie.o: CB_custom_cookie.h
CB_custom_feature.o: CB_custom_feature.h CB_vars_base.h CB_finger.h
CB_custom_postvars.o: CB_custom_postvars.h
CB_fgp_info.o: CB_fgp_info.h CB_finger.h CB_wgt_prim.h CB_config.h
CB_finger.o: CB_finger.h CB_snippet.h CB_wgt_prim.h CB_ua_conf.h
CB_snippet.o: CB_snippet.h
CB_ua_conf.o: CB_ua_conf.h CB_wgt_prim_db.h
CB_ua_info.o: CB_ua_info.h
CB_vars_base.o: CB_vars_base.h
CB_virtualizer.o: CB_virtualizer.h CB_config.h CB_fgp_info.h CB_ua_conf.h CB_snippet.h CB_wgt_prim.h CB_finger.h CB_vars_base.h
CB_wgt_prim.o: CB_wgt_prim.h
CB_wgt_prim_db.o: CB_wgt_prim_db.h
main.o: CB_virtualizer.h CB_fgp_info.h CB_ua_info.h CB_config.h CB_custom_automatic.h CB_custom_postvars.h CB_custom_feature.h CB_custom_cookie.h
