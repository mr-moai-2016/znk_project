# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)\..\mkfsys
endif

ifeq ($(MACHINE), x64)
  PLATFORM=win64
else
  MACHINE=x86
  PLATFORM=win32
endif
# Output directory
ABINAME=mingw$(MACHINE)$(DEBUG)
O = .\out_dir\$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=gcc -Wall -Wstrict-aliasing=2 -g
LINKER=gcc
DLIBS_DIR=dlib\$(PLATFORM)_mingwd
SLIBS_DIR=slib\$(PLATFORM)_mingwd
else
COMPILER=gcc -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG
LINKER=gcc
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
endif

CP=xcopy /H /C /Y

INCLUDE_FLAG+=  \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \


include Makefile_version.mak

BASENAME0=custom_boy
EXE_FILE0=$O\custom_boy.cgi
OBJS0=\
	$O\CB_config.o \
	$O\CB_custom_automatic.o \
	$O\CB_custom_cookie.o \
	$O\CB_custom_feature.o \
	$O\CB_custom_postvars.o \
	$O\CB_fgp_info.o \
	$O\CB_finger.o \
	$O\CB_snippet.o \
	$O\CB_ua_conf.o \
	$O\CB_ua_info.o \
	$O\CB_vars_base.o \
	$O\CB_virtualizer.o \
	$O\CB_wgt_prim.o \
	$O\CB_wgt_prim_db.o \
	$O\main.o \

SUB_LIBS=\

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
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) 
	$(LINKER) -o $(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 


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
$O\\%.o: $S\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\%.o: $S\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\templates @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\templates 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\publicbox @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\publicbox 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\UA @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\UA 
	@if exist "custom_boy.myf" @$(CP) /F "custom_boy.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\ $(CP_END)
	@if exist "templates\*.html" @$(CP) /F "templates\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\templates\ $(CP_END)
	@if exist "publicbox\*" @$(CP) /F "publicbox\*" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\publicbox\ $(CP_END)
	@if exist "RE\*.png" @$(CP) /F "RE\*.png" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE\ $(CP_END)
	@if exist "RE\*.myf" @$(CP) /F "RE\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE\ $(CP_END)
	@if exist "UA\*.myf" @$(CP) /F "UA\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\UA\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0)
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\ $(CP_END)

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	del /Q $O\ 

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
