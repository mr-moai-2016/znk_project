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

BASENAME0=ssl
DLIB_NAME0=libssl-45.so
DLIB_FILE0=$O/$(DLIB_NAME0)
ILIB_FILE0=$O/libssl-45.so
SLIB_FILE0=$O/libssl.a
OBJS0=\
	$O/t1_enc.o \
	$O/ssl_init.o \
	$O/ssl_srvr.o \
	$O/d1_pkt.o \
	$O/ssl_versions.o \
	$O/t1_hash.o \
	$O/t1_srvr.o \
	$O/ssl_txt.o \
	$O/s3_cbc.o \
	$O/ssl_both.o \
	$O/t1_meth.o \
	$O/ssl_cert.o \
	$O/d1_both.o \
	$O/ssl_err.o \
	$O/ssl_rsa.o \
	$O/ssl_packet.o \
	$O/ssl_algs.o \
	$O/bs_ber.o \
	$O/d1_srvr.o \
	$O/ssl_lib.o \
	$O/ssl_clnt.o \
	$O/ssl_asn1.o \
	$O/bs_cbs.o \
	$O/d1_lib.o \
	$O/ssl_pkt.o \
	$O/d1_meth.o \
	$O/ssl_ciph.o \
	$O/ssl_stat.o \
	$O/d1_enc.o \
	$O/pqueue.o \
	$O/d1_srtp.o \
	$O/t1_clnt.o \
	$O/bs_cbb.o \
	$O/bio_ssl.o \
	$O/s3_lib.o \
	$O/ssl_tlsext.o \
	$O/ssl_sess.o \
	$O/d1_clnt.o \
	$O/t1_lib.o \
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
all: $O $(DLIB_FILE0) $(SLIB_FILE0) 

# Mkdir rule.
$O:
	mkdir -p $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	if test -e $(SLIB_FILE0) ; then rm -f $(SLIB_FILE0); fi
	@echo ar cru $(SLIB_FILE0) {[objs]} $(SUB_OBJS_ECHO)
	@     ar cru $(SLIB_FILE0) $(OBJS0) $(SUB_OBJS)
	ranlib $(SLIB_FILE0)

gsl.myf: $(SLIB_FILE0)
	if test -e $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe ; then $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE) ; fi

gsl.def: gsl.myf
	if test -e $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe ; then $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe -d gsl.myf gsl.def ; fi

$(DLIB_FILE0): $(OBJS0) $(SLIB_FILE0)
	if test -e $(DLIB_FILE0) ; then rm -f $(DLIB_FILE0); fi
	@echo $(GCC_CMD) -shared -Wl,-soname,$(DLIB_NAME0) -o $(DLIB_FILE0) {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.a -lpthread -ldl -lstdc++  -lc
	@     $(GCC_CMD) -shared -Wl,-soname,$(DLIB_NAME0) -o $(DLIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.a -lpthread -ldl -lstdc++  -lc
	if [ x"$O" != x ]; then /sbin/ldconfig -n $O; else /sbin/ldconfig -n .; fi


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
t1_enc.o: ssl_locl.h
ssl_init.o: ssl_locl.h
ssl_srvr.o: ssl_locl.h bytestring.h ssl_tlsext.h
d1_pkt.o: ssl_locl.h bytestring.h
ssl_versions.o: ssl_locl.h
t1_hash.o: ssl_locl.h
t1_srvr.o: ssl_locl.h
ssl_txt.o: ssl_locl.h
s3_cbc.o: ssl_locl.h
ssl_both.o: ssl_locl.h bytestring.h
t1_meth.o: ssl_locl.h
ssl_cert.o: ssl_locl.h
d1_both.o: ssl_locl.h bytestring.h
ssl_err.o: ssl_locl.h
ssl_rsa.o: ssl_locl.h
ssl_packet.o: ssl_locl.h bytestring.h
ssl_algs.o: ssl_locl.h
bs_ber.o: bytestring.h
d1_srvr.o: ssl_locl.h
ssl_lib.o: ssl_locl.h bytestring.h
ssl_clnt.o: ssl_locl.h bytestring.h ssl_tlsext.h
ssl_asn1.o: ssl_locl.h bytestring.h
bs_cbs.o: bytestring.h
d1_lib.o: ssl_locl.h
ssl_pkt.o: ssl_locl.h bytestring.h
d1_meth.o: ssl_locl.h
ssl_ciph.o: ssl_locl.h
ssl_stat.o: ssl_locl.h
d1_enc.o: ssl_locl.h
d1_srtp.o: ssl_locl.h bytestring.h srtp.h
t1_clnt.o: ssl_locl.h
bs_cbb.o: bytestring.h
bio_ssl.o: ssl_locl.h
s3_lib.o: ssl_locl.h bytestring.h
ssl_tlsext.o: ssl_locl.h bytestring.h ssl_tlsext.h
ssl_sess.o: ssl_locl.h
d1_clnt.o: ssl_locl.h bytestring.h
t1_lib.o: ssl_locl.h bytestring.h ssl_tlsext.h
