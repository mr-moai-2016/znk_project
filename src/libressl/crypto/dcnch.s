# IMAGE_IMPORT_DESCRIPTOR
	.section	.idata$2
	.global	__head___out_dir_mingwx86_libcrypto_2_7_2_dll_a
__head___out_dir_mingwx86_libcrypto_2_7_2_dll_a:
	.rva	hname	#Ptr to image import by name list
	#this should be the timestamp, but NT sometimes
	#doesn't load DLLs when this is set.
	.long	0	# loaded time
	.long	0	# Forwarder chain
	.rva	____out_dir_mingwx86_libcrypto_2_7_2_dll_a_iname	# imported dll's name
	.rva	fthunk	# pointer to firstthunk
#Stuff for compatibility
	.section	.idata$5
	.long	0
fthunk:
	.section	.idata$4
	.long	0
	.section	.idata$4
hname:
