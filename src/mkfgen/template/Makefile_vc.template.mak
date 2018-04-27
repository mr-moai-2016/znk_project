# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=$[my_libs_root]$
!IFNDEF MKFSYS_DIR
MKFSYS_DIR=$(MY_LIBS_ROOT)\..\mkfsys
!ENDIF

!IF "$(MACHINE)" == "x64"
VC_MACHINE=AMD64
PLATFORM=win64
!ELSE
MACHINE=x86
VC_MACHINE=X86
PLATFORM=win32
!ENDIF
# Output directory
ABINAME=vc$(MACHINE)$(DEBUG)
O = .\out_dir\$(ABINAME)

!IF "$(DEBUG)" == "d"
COMPILER=cl -nologo -EHsc /GR /W4 /MTd /Z7 /D_DEBUG /RTC1 $[compiler_option_special]$
LINKER=LINK /MACHINE:$(VC_MACHINE) /DEBUG /INCREMENTAL:NO /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)_vcd
SLIBS_DIR=slib\$(PLATFORM)_vcd
!ELSE
COMPILER=cl -nologo -EHsc /GR /W4 /MT /O2 /DNDEBUG $[compiler_option_special]$
LINKER=LINK /MACHINE:$(VC_MACHINE) /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
!ENDIF

CP=xcopy /H /C /Y

INCLUDE_FLAG =  \
$[include_flags]$

# We cannot use variables in include of nmake.
include Makefile_version.mak

$[objs_defs]$

# Entry rule.
$[all_target_rule]$
# Mkdir rule.
$[mkdir_rule]$
# Product files rule.
$[product_files_rule]$
# Suffix rule.
$[suffix_rule]$
# Rc rule.
$[rc_rule]$
# Submkf rule.
$[submkf_rule]$
# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
$[install_data_rule]$
# Install exec rule.
$[install_exec_rule]$
# Install dlib rule.
$[install_dlib_rule]$
# Install slib rule.
$[install_slib_rule]$
# Install rule.
$[install_rule]$
# Clean rule.
$[clean_rule]$
