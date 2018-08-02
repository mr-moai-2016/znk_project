# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=../..
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
PLATFORM=cygwin
ifeq ($(MACHINE), x64)
  GCC_CMD=gcc -m64
  PLATFORM=cygwin64
endif
ifeq ($(MACHINE), x86)
  GCC_CMD=gcc -m32
  PLATFORM=cygwin32
endif
# Output directory
ABINAME = cygwin$(MACHINE)$(DEBUG)
O = ./out_dir/$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -g -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)d
SLIBS_DIR=slib/$(PLATFORM)d
else
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)
SLIBS_DIR=slib/$(PLATFORM)
endif

CP=cp

INCLUDE_FLAG+=  \
	-I../include \
	-I../include/compat \
	-I../crypto/asn1 \
	-I../crypto/bn \
	-I../crypto/evp \
	-I../crypto/modes \
	-I../crypto \


include Makefile_version.mak

BASENAME0=tls
DLIB_NAME0=cygtls-17.dll
DLIB_FILE0=$O/$(DLIB_NAME0)
ILIB_FILE0=$O/cygtls-17.dll
SLIB_FILE0=$O/libtls.a
OBJS0=\
	$O/tls_conninfo.o \
	$O/tls_verify.o \
	$O/tls_bio_cb.o \
	$O/tls_config.o \
	$O/tls_keypair.o \
	$O/tls_server.o \
	$O/tls_peer.o \
	$O/compat/pwrite.o \
	$O/compat/getuid.o \
	$O/compat/pread.o \
	$O/compat/ftruncate.o \
	$O/tls_client.o \
	$O/tls_ocsp.o \
	$O/tls.o \
	$O/tls_util.o \
	$O/dll_main.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_DLIBS= \
	__mkg_sentinel_target__ \
	$(DLIB_FILE0) \

PRODUCT_SLIBS= \
	__mkg_sentinel_target__ \
	$(SLIB_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \



# Entry rule.
all: $O/compat $O $(DLIB_FILE0) $(SLIB_FILE0) 

# Mkdir rule.
$O/compat:
	mkdir -p $O/compat

$O:
	mkdir -p $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	if test -e $(SLIB_FILE0) ; then rm -f $(SLIB_FILE0) ; fi
	@echo ar cru $(SLIB_FILE0) {[objs]} $(SUB_OBJS_ECHO)
	@     ar cru $(SLIB_FILE0) $(OBJS0) $(SUB_OBJS)
	ranlib $(SLIB_FILE0)

gsl.myf: $(SLIB_FILE0)
	if test -e $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe ; then $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE) ; fi

gsl.def: gsl.myf
	if test -e $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe ; then $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe -d gsl.myf gsl.def ; fi

$(DLIB_FILE0): $(OBJS0) $(SLIB_FILE0)
	if test -e $(DLIB_FILE0) ; then rm -f $(DLIB_FILE0); fi
	@echo gcc -shared -o $(DLIB_FILE0) {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libssl.a $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.a -lpthread -ldl -lstdc++  -lc
	@     gcc -shared -o $(DLIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libssl.a $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.a -lpthread -ldl -lstdc++  -lc


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
$O/compat/%.o: $S/compat/%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O/compat/%.o: $S/compat/%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

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

# Install exec rule.
install_exec:
	@for tgt in $(RUNTIME_FILES) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" $(MY_LIBS_ROOT)/$(DLIBS_DIR)/ ; fi ; done

# Install dlib rule.
install_dlib: $(DLIB_FILE0)
	mkdir -p $(MY_LIBS_ROOT)/$(DLIBS_DIR) 
	for tgt in $(DLIB_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" $(MY_LIBS_ROOT)/$(DLIBS_DIR)/ ; fi ; done

# Install slib rule.
install_slib: $(SLIB_FILE0)
	mkdir -p $(MY_LIBS_ROOT)/$(SLIBS_DIR) 
	for tgt in $(SLIB_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" $(MY_LIBS_ROOT)/$(SLIBS_DIR)/ ; fi ; done

# Install rule.
install: all install_slib install_dlib 


# Clean rule.
clean:
	rm -rf $O/ 

# Src and Headers Dependency
tls_conninfo.o: tls_internal.h
tls_verify.o: tls_internal.h
tls_bio_cb.o: tls_internal.h
tls_config.o: tls_internal.h
tls_keypair.o: tls_internal.h
tls_server.o: tls_internal.h
tls_peer.o: tls_internal.h
tls_client.o: tls_internal.h
tls_ocsp.o: tls_internal.h
tls.o: tls_internal.h
tls_util.o: tls_internal.h
