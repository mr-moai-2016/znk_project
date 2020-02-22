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

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Znk-$(DL_VER).dll \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Rano-$(DL_VER).dll \



# Entry rule.
all: $O $(EXE_FILE0) 

# Mkdir rule.
$O:
	mkdir -p $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) 
	@echo $(LINKER) -o $(EXE_FILE0)  {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 
	@     $(LINKER) -o $(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 


##
# Pattern rule.
#
# We use not suffix rule but pattern rule for dealing flexibly with files in sub-directory.
# In this case, there is very confusing specification, that is :
# '\' to the left hand of ':' works as escape sequence, 
# '\' to the right hand of ':' does not work as escape sequence. 
# Hence, we have to duplicate '\' to the left hand of ':',
# the other way, '\' to the right hand of ':' we have to put only single '\'.
# Note that we have to duplicate '\' only before special charactor(% etc) in the left of ':'.
#
# For example 1 :
#   $O\\mydir\\%.o: $S\%.c        .... NG
#   $O\mydir\\%.o:  $S\%.c        .... OK
# For example 2 :
#   $O\\mydir\%.o:  $S\mydir\%.c  .... NG
#   $O\mydir\\%.o:  $S\mydir\%.c  .... OK
# In the case of example 2, we can write more simply :
#   $O\\%.o: $S\%.c               .... OK
#   (Because '%' is wildcard and it indicates patical path 'mydir\filename_base' recursively )
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
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/templates 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/publicbox 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/RE 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/UA 
	for tgt in custom_boy.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/ ; fi ; done
	for tgt in rano_app.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/custom_boy/ ; fi ; done
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
	rm -rf $O/ 

# Src and Headers Dependency
$O/CB_config.o: CB_config.h
$O/CB_custom_automatic.o: CB_custom_automatic.h
$O/CB_custom_cookie.o: CB_custom_cookie.h
$O/CB_custom_feature.o: CB_custom_feature.h CB_vars_base.h CB_finger.h
$O/CB_custom_postvars.o: CB_custom_postvars.h
$O/CB_fgp_info.o: CB_fgp_info.h CB_finger.h CB_wgt_prim.h CB_config.h
$O/CB_finger.o: CB_finger.h CB_snippet.h CB_wgt_prim.h CB_ua_conf.h
$O/CB_snippet.o: CB_snippet.h
$O/CB_ua_conf.o: CB_ua_conf.h CB_wgt_prim_db.h
$O/CB_ua_info.o: CB_ua_info.h
$O/CB_vars_base.o: CB_vars_base.h
$O/CB_virtualizer.o: CB_virtualizer.h CB_config.h CB_fgp_info.h CB_ua_conf.h CB_snippet.h CB_wgt_prim.h CB_finger.h CB_vars_base.h
$O/CB_wgt_prim.o: CB_wgt_prim.h
$O/CB_wgt_prim_db.o: CB_wgt_prim_db.h
$O/main.o: CB_virtualizer.h CB_fgp_info.h CB_ua_info.h CB_config.h CB_custom_automatic.h CB_custom_postvars.h CB_custom_feature.h CB_custom_cookie.h
