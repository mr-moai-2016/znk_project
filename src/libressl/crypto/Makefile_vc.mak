# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..\..
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
COMPILER=cl -nologo -EHsc /GR /W4 /MTd /Z7 /D_DEBUG /RTC1 -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM \
	/wd4706 /wd4996
LINKER=LINK /MACHINE:$(VC_MACHINE) /DEBUG /INCREMENTAL:NO /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)_vcd
SLIBS_DIR=slib\$(PLATFORM)_vcd
!ELSE
COMPILER=cl -nologo -EHsc /GR /W4 /MT /O2 /DNDEBUG -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM \
	/wd4706 /wd4996
LINKER=LINK /MACHINE:$(VC_MACHINE) /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
!ENDIF

CP=xcopy /H /C /Y

INCLUDE_FLAG =  \
	-I../include \
	-I../include/compat \
	-I../crypto/asn1 \
	-I../crypto/bn \
	-I../crypto/evp \
	-I../crypto/modes \
	-I../crypto \


# We cannot use variables in include of nmake.
include Makefile_version.mak

BASENAME0=crypto
DLIB_NAME0=libcrypto-43.dll
DLIB_FILE0=$O\$(DLIB_NAME0)
ILIB_FILE0=$O\libcrypto-43.imp.lib
SLIB_FILE0=$O\libcrypto.lib
OBJS0=\
	$O\ts/ts_asn1.obj \
	$O\ts/ts_req_utils.obj \
	$O\ts/ts_rsp_sign.obj \
	$O\ts/ts_conf.obj \
	$O\ts/ts_rsp_print.obj \
	$O\ts/ts_req_print.obj \
	$O\ts/ts_err.obj \
	$O\ts/ts_rsp_utils.obj \
	$O\ts/ts_rsp_verify.obj \
	$O\ts/ts_verify_ctx.obj \
	$O\ts/ts_lib.obj \
	$O\dsa/dsa_ossl.obj \
	$O\dsa/dsa_sign.obj \
	$O\dsa/dsa_gen.obj \
	$O\dsa/dsa_meth.obj \
	$O\dsa/dsa_lib.obj \
	$O\dsa/dsa_depr.obj \
	$O\dsa/dsa_prn.obj \
	$O\dsa/dsa_err.obj \
	$O\dsa/dsa_pmeth.obj \
	$O\dsa/dsa_vrf.obj \
	$O\dsa/dsa_ameth.obj \
	$O\dsa/dsa_key.obj \
	$O\dsa/dsa_asn1.obj \
	$O\chacha/chacha.obj \
	$O\ecdsa/ecs_vrf.obj \
	$O\ecdsa/ecs_sign.obj \
	$O\ecdsa/ecs_ossl.obj \
	$O\ecdsa/ecs_err.obj \
	$O\ecdsa/ecs_asn1.obj \
	$O\ecdsa/ecs_lib.obj \
	$O\rc2/rc2_skey.obj \
	$O\rc2/rc2ofb64.obj \
	$O\rc2/rc2cfb64.obj \
	$O\rc2/rc2_ecb.obj \
	$O\rc2/rc2_cbc.obj \
	$O\whrlpool/wp_block.obj \
	$O\whrlpool/wp_dgst.obj \
	$O\x509v3/v3_ia5.obj \
	$O\x509v3/v3_pcons.obj \
	$O\x509v3/pcy_map.obj \
	$O\x509v3/v3_pci.obj \
	$O\x509v3/v3_pcia.obj \
	$O\x509v3/v3_ocsp.obj \
	$O\x509v3/v3_skey.obj \
	$O\x509v3/v3_purp.obj \
	$O\x509v3/v3_sxnet.obj \
	$O\x509v3/v3_pku.obj \
	$O\x509v3/v3_lib.obj \
	$O\x509v3/v3_crld.obj \
	$O\x509v3/v3_enum.obj \
	$O\x509v3/v3_alt.obj \
	$O\x509v3/v3_bcons.obj \
	$O\x509v3/pcy_data.obj \
	$O\x509v3/pcy_cache.obj \
	$O\x509v3/v3_int.obj \
	$O\x509v3/v3_genn.obj \
	$O\x509v3/v3_akeya.obj \
	$O\x509v3/v3_bitst.obj \
	$O\x509v3/v3_utl.obj \
	$O\x509v3/v3_prn.obj \
	$O\x509v3/pcy_tree.obj \
	$O\x509v3/v3_ncons.obj \
	$O\x509v3/v3_info.obj \
	$O\x509v3/v3_cpols.obj \
	$O\x509v3/v3err.obj \
	$O\x509v3/v3_extku.obj \
	$O\x509v3/v3_akey.obj \
	$O\x509v3/pcy_node.obj \
	$O\x509v3/v3_conf.obj \
	$O\x509v3/pcy_lib.obj \
	$O\x509v3/v3_pmaps.obj \
	$O\bf/bf_skey.obj \
	$O\bf/bf_enc.obj \
	$O\bf/bf_ecb.obj \
	$O\bf/bf_cfb64.obj \
	$O\bf/bf_ofb64.obj \
	$O\gost/gostr341001_ameth.obj \
	$O\gost/gostr341001_pmeth.obj \
	$O\gost/gost_err.obj \
	$O\gost/gostr341001.obj \
	$O\gost/gost_asn1.obj \
	$O\gost/gost89imit_ameth.obj \
	$O\gost/gostr341194.obj \
	$O\gost/gostr341001_key.obj \
	$O\gost/gost89imit_pmeth.obj \
	$O\gost/gost89_params.obj \
	$O\gost/streebog.obj \
	$O\gost/gost2814789.obj \
	$O\gost/gost89_keywrap.obj \
	$O\gost/gostr341001_params.obj \
	$O\idea/i_cfb64.obj \
	$O\idea/i_skey.obj \
	$O\idea/i_cbc.obj \
	$O\idea/i_ofb64.obj \
	$O\idea/i_ecb.obj \
	$O\buffer/buffer.obj \
	$O\buffer/buf_str.obj \
	$O\buffer/buf_err.obj \
	$O\o_time.obj \
	$O\cpt_err.obj \
	$O\cryptlib.obj \
	$O\modes/xts128.obj \
	$O\modes/ccm128.obj \
	$O\modes/gcm128.obj \
	$O\modes/ofb128.obj \
	$O\modes/cts128.obj \
	$O\modes/cbc128.obj \
	$O\modes/ctr128.obj \
	$O\modes/cfb128.obj \
	$O\rra_portable/inet_aton.obj \
	$O\rra_portable/getnameinfo.obj \
	$O\rra_portable/snprintf.obj \
	$O\rra_portable/inet_ntoa.obj \
	$O\rra_portable/getaddrinfo.obj \
	$O\rra_portable/inet_ntop.obj \
	$O\rra_portable/inet_pton.obj \
	$O\md4/md4_dgst.obj \
	$O\md4/md4_one.obj \
	$O\dso/dso_lib.obj \
	$O\dso/dso_dlfcn.obj \
	$O\dso/dso_openssl.obj \
	$O\dso/dso_null.obj \
	$O\dso/dso_err.obj \
	$O\hkdf/hkdf.obj \
	$O\stack/stack.obj \
	$O\txt_db/txt_db.obj \
	$O\bn/bn_mpi.obj \
	$O\bn/bn_asm.obj \
	$O\bn/bn_prime.obj \
	$O\bn/bn_const.obj \
	$O\bn/bn_depr.obj \
	$O\bn/bn_nist.obj \
	$O\bn/bn_gf2m.obj \
	$O\bn/bn_sqr.obj \
	$O\bn/bn_rand.obj \
	$O\bn/bn_err.obj \
	$O\bn/bn_ctx.obj \
	$O\bn/bn_lib.obj \
	$O\bn/bn_mont.obj \
	$O\bn/bn_mod.obj \
	$O\bn/bn_mul.obj \
	$O\bn/bn_recp.obj \
	$O\bn/bn_kron.obj \
	$O\bn/bn_gcd.obj \
	$O\bn/bn_exp2.obj \
	$O\bn/bn_x931p.obj \
	$O\bn/bn_div.obj \
	$O\bn/bn_shift.obj \
	$O\bn/bn_add.obj \
	$O\bn/bn_word.obj \
	$O\bn/bn_exp.obj \
	$O\bn/bn_sqrt.obj \
	$O\bn/bn_print.obj \
	$O\bn/bn_blind.obj \
	$O\dh/dh_depr.obj \
	$O\dh/dh_key.obj \
	$O\dh/dh_check.obj \
	$O\dh/dh_pmeth.obj \
	$O\dh/dh_gen.obj \
	$O\dh/dh_prn.obj \
	$O\dh/dh_asn1.obj \
	$O\dh/dh_err.obj \
	$O\dh/dh_ameth.obj \
	$O\dh/dh_lib.obj \
	$O\ripemd/rmd_one.obj \
	$O\ripemd/rmd_dgst.obj \
	$O\conf/conf_sap.obj \
	$O\conf/conf_def.obj \
	$O\conf/conf_lib.obj \
	$O\conf/conf_mall.obj \
	$O\conf/conf_mod.obj \
	$O\conf/conf_err.obj \
	$O\conf/conf_api.obj \
	$O\ecdh/ech_lib.obj \
	$O\ecdh/ech_err.obj \
	$O\ecdh/ech_key.obj \
	$O\mem_clr.obj \
	$O\err/err_prn.obj \
	$O\err/err.obj \
	$O\err/err_all.obj \
	$O\pkcs7/pk7_attr.obj \
	$O\pkcs7/pkcs7err.obj \
	$O\pkcs7/pk7_asn1.obj \
	$O\pkcs7/pk7_doit.obj \
	$O\pkcs7/pk7_smime.obj \
	$O\pkcs7/pk7_mime.obj \
	$O\pkcs7/bio_pk7.obj \
	$O\pkcs7/pk7_lib.obj \
	$O\comp/comp_err.obj \
	$O\comp/comp_lib.obj \
	$O\comp/c_rle.obj \
	$O\comp/c_zlib.obj \
	$O\lhash/lh_stats.obj \
	$O\lhash/lhash.obj \
	$O\malloc-wrapper.obj \
	$O\hmac/hmac.obj \
	$O\hmac/hm_ameth.obj \
	$O\hmac/hm_pmeth.obj \
	$O\rand/rand_lib.obj \
	$O\rand/randfile.obj \
	$O\rand/rand_err.obj \
	$O\ocsp/ocsp_prn.obj \
	$O\ocsp/ocsp_err.obj \
	$O\ocsp/ocsp_ext.obj \
	$O\ocsp/ocsp_vfy.obj \
	$O\ocsp/ocsp_ht.obj \
	$O\ocsp/ocsp_cl.obj \
	$O\ocsp/ocsp_lib.obj \
	$O\ocsp/ocsp_srv.obj \
	$O\ocsp/ocsp_asn.obj \
	$O\x509/x509spki.obj \
	$O\x509/x509_v3.obj \
	$O\x509/by_dir.obj \
	$O\x509/x509_r2x.obj \
	$O\x509/x509rset.obj \
	$O\x509/x509_lu.obj \
	$O\x509/x509_err.obj \
	$O\x509/x509_d2.obj \
	$O\x509/x509_def.obj \
	$O\x509/x509_vfy.obj \
	$O\x509/x_all.obj \
	$O\x509/by_mem.obj \
	$O\x509/x509_att.obj \
	$O\x509/x509_ext.obj \
	$O\x509/x509_set.obj \
	$O\x509/x509_vpm.obj \
	$O\x509/x509_txt.obj \
	$O\x509/x509name.obj \
	$O\x509/by_file.obj \
	$O\x509/x509_req.obj \
	$O\x509/x509type.obj \
	$O\x509/x509cset.obj \
	$O\x509/x509_obj.obj \
	$O\x509/x509_trs.obj \
	$O\x509/x509_cmp.obj \
	$O\ui/ui_err.obj \
	$O\ui/ui_util.obj \
	$O\ui/ui_lib.obj \
	$O\crypto_init.obj \
	$O\o_init.obj \
	$O\cversion.obj \
	$O\poly1305/poly1305.obj \
	$O\ex_data.obj \
	$O\rc4/rc4_skey.obj \
	$O\rc4/rc4_enc.obj \
	$O\bio/bss_dgram.obj \
	$O\bio/bss_mem.obj \
	$O\bio/b_print.obj \
	$O\bio/bss_null.obj \
	$O\bio/bio_cb.obj \
	$O\bio/bio_lib.obj \
	$O\bio/bio_err.obj \
	$O\bio/b_dump.obj \
	$O\bio/bss_conn.obj \
	$O\bio/bf_buff.obj \
	$O\bio/bf_null.obj \
	$O\bio/bf_nbio.obj \
	$O\bio/bss_acpt.obj \
	$O\bio/b_sock.obj \
	$O\bio/bss_sock.obj \
	$O\bio/bss_file.obj \
	$O\bio/bss_bio.obj \
	$O\bio/bss_fd.obj \
	$O\bio/bio_meth.obj \
	$O\sha/sha256.obj \
	$O\sha/sha1dgst.obj \
	$O\sha/sha512.obj \
	$O\sha/sha1_one.obj \
	$O\des/set_key.obj \
	$O\des/pcbc_enc.obj \
	$O\des/ofb_enc.obj \
	$O\des/fcrypt.obj \
	$O\des/cbc_cksm.obj \
	$O\des/qud_cksm.obj \
	$O\des/ecb_enc.obj \
	$O\des/fcrypt_b.obj \
	$O\des/rand_key.obj \
	$O\des/ofb64ede.obj \
	$O\des/enc_read.obj \
	$O\des/cbc_enc.obj \
	$O\des/xcbc_enc.obj \
	$O\des/ecb3_enc.obj \
	$O\des/enc_writ.obj \
	$O\des/cfb64ede.obj \
	$O\des/des_enc.obj \
	$O\des/ede_cbcm_enc.obj \
	$O\des/cfb_enc.obj \
	$O\des/ofb64enc.obj \
	$O\des/str2key.obj \
	$O\des/cfb64enc.obj \
	$O\camellia/cmll_ofb.obj \
	$O\camellia/cmll_ctr.obj \
	$O\camellia/cmll_cbc.obj \
	$O\camellia/camellia.obj \
	$O\camellia/cmll_ecb.obj \
	$O\camellia/cmll_misc.obj \
	$O\camellia/cmll_cfb.obj \
	$O\rsa/rsa_lib.obj \
	$O\rsa/rsa_chk.obj \
	$O\rsa/rsa_depr.obj \
	$O\rsa/rsa_eay.obj \
	$O\rsa/rsa_crpt.obj \
	$O\rsa/rsa_err.obj \
	$O\rsa/rsa_oaep.obj \
	$O\rsa/rsa_x931.obj \
	$O\rsa/rsa_pss.obj \
	$O\rsa/rsa_saos.obj \
	$O\rsa/rsa_pk1.obj \
	$O\rsa/rsa_meth.obj \
	$O\rsa/rsa_asn1.obj \
	$O\rsa/rsa_gen.obj \
	$O\rsa/rsa_sign.obj \
	$O\rsa/rsa_prn.obj \
	$O\rsa/rsa_ameth.obj \
	$O\rsa/rsa_pmeth.obj \
	$O\rsa/rsa_none.obj \
	$O\compat/freezero.obj \
	$O\compat/strndup.obj \
	$O\compat/bsd-asprintf.obj \
	$O\compat/strlcpy.obj \
	$O\compat/timegm.obj \
	$O\compat/strnlen.obj \
	$O\compat/recallocarray.obj \
	$O\compat/reallocarray.obj \
	$O\compat/timingsafe_bcmp.obj \
	$O\compat/strsep.obj \
	$O\compat/timingsafe_memcmp.obj \
	$O\compat/strlcat.obj \
	$O\cmac/cm_pmeth.obj \
	$O\cmac/cmac.obj \
	$O\cmac/cm_ameth.obj \
	$O\asn1/x_pubkey.obj \
	$O\asn1/a_time_tm.obj \
	$O\asn1/asn1_gen.obj \
	$O\asn1/evp_asn1.obj \
	$O\asn1/bio_ndef.obj \
	$O\asn1/t_crl.obj \
	$O\asn1/tasn_prn.obj \
	$O\asn1/x_x509a.obj \
	$O\asn1/x_sig.obj \
	$O\asn1/asn1_lib.obj \
	$O\asn1/a_mbstr.obj \
	$O\asn1/t_spki.obj \
	$O\asn1/a_digest.obj \
	$O\asn1/asn_moid.obj \
	$O\asn1/bio_asn1.obj \
	$O\asn1/a_strnid.obj \
	$O\asn1/i2d_pr.obj \
	$O\asn1/ameth_lib.obj \
	$O\asn1/tasn_typ.obj \
	$O\asn1/nsseq.obj \
	$O\asn1/a_i2d_fp.obj \
	$O\asn1/t_pkey.obj \
	$O\asn1/x_attrib.obj \
	$O\asn1/a_set.obj \
	$O\asn1/tasn_fre.obj \
	$O\asn1/x_bignum.obj \
	$O\asn1/x_info.obj \
	$O\asn1/a_dup.obj \
	$O\asn1/a_d2i_fp.obj \
	$O\asn1/a_verify.obj \
	$O\asn1/p8_pkey.obj \
	$O\asn1/tasn_utl.obj \
	$O\asn1/a_sign.obj \
	$O\asn1/a_object.obj \
	$O\asn1/a_bytes.obj \
	$O\asn1/x_nx509.obj \
	$O\asn1/x_exten.obj \
	$O\asn1/x_val.obj \
	$O\asn1/t_req.obj \
	$O\asn1/n_pkey.obj \
	$O\asn1/x_long.obj \
	$O\asn1/a_print.obj \
	$O\asn1/a_time.obj \
	$O\asn1/d2i_pu.obj \
	$O\asn1/asn1_par.obj \
	$O\asn1/x_spki.obj \
	$O\asn1/x_pkey.obj \
	$O\asn1/a_enum.obj \
	$O\asn1/x_req.obj \
	$O\asn1/i2d_pu.obj \
	$O\asn1/a_octet.obj \
	$O\asn1/tasn_dec.obj \
	$O\asn1/tasn_enc.obj \
	$O\asn1/t_x509a.obj \
	$O\asn1/tasn_new.obj \
	$O\asn1/p5_pbev2.obj \
	$O\asn1/a_strex.obj \
	$O\asn1/t_x509.obj \
	$O\asn1/x_crl.obj \
	$O\asn1/asn_mime.obj \
	$O\asn1/a_bitstr.obj \
	$O\asn1/asn1_err.obj \
	$O\asn1/x_x509.obj \
	$O\asn1/a_type.obj \
	$O\asn1/x_algor.obj \
	$O\asn1/d2i_pr.obj \
	$O\asn1/p5_pbe.obj \
	$O\asn1/a_int.obj \
	$O\asn1/asn_pack.obj \
	$O\asn1/x_name.obj \
	$O\asn1/a_utf8.obj \
	$O\asn1/a_bool.obj \
	$O\asn1/f_int.obj \
	$O\asn1/f_enum.obj \
	$O\asn1/f_string.obj \
	$O\asn1/t_bitst.obj \
	$O\mem_dbg.obj \
	$O\curve25519/curve25519.obj \
	$O\curve25519/curve25519-generic.obj \
	$O\md5/md5_one.obj \
	$O\md5/md5_dgst.obj \
	$O\pem/pem_pkey.obj \
	$O\pem/pem_lib.obj \
	$O\pem/pem_xaux.obj \
	$O\pem/pem_pk8.obj \
	$O\pem/pem_info.obj \
	$O\pem/pem_seal.obj \
	$O\pem/pem_all.obj \
	$O\pem/pem_sign.obj \
	$O\pem/pem_oth.obj \
	$O\pem/pvkfmt.obj \
	$O\pem/pem_err.obj \
	$O\pem/pem_x509.obj \
	$O\evp/e_aes.obj \
	$O\evp/m_ripemd.obj \
	$O\evp/e_chacha.obj \
	$O\evp/p_enc.obj \
	$O\evp/m_md5.obj \
	$O\evp/pmeth_gn.obj \
	$O\evp/bio_md.obj \
	$O\evp/p_lib.obj \
	$O\evp/evp_pkey.obj \
	$O\evp/e_rc2.obj \
	$O\evp/p_open.obj \
	$O\evp/m_ecdsa.obj \
	$O\evp/e_idea.obj \
	$O\evp/e_bf.obj \
	$O\evp/p_dec.obj \
	$O\evp/evp_err.obj \
	$O\evp/m_sigver.obj \
	$O\evp/p_seal.obj \
	$O\evp/m_dss1.obj \
	$O\evp/bio_b64.obj \
	$O\evp/p5_crpt2.obj \
	$O\evp/bio_enc.obj \
	$O\evp/p_sign.obj \
	$O\evp/e_des.obj \
	$O\evp/e_cast.obj \
	$O\evp/pmeth_lib.obj \
	$O\evp/m_dss.obj \
	$O\evp/m_md5_sha1.obj \
	$O\evp/e_chacha20poly1305.obj \
	$O\evp/evp_pbe.obj \
	$O\evp/encode.obj \
	$O\evp/m_md4.obj \
	$O\evp/c_all.obj \
	$O\evp/names.obj \
	$O\evp/p5_crpt.obj \
	$O\evp/m_gostr341194.obj \
	$O\evp/evp_aead.obj \
	$O\evp/e_old.obj \
	$O\evp/evp_key.obj \
	$O\evp/evp_enc.obj \
	$O\evp/e_gost2814789.obj \
	$O\evp/e_rc4_hmac_md5.obj \
	$O\evp/e_aes_cbc_hmac_sha1.obj \
	$O\evp/e_des3.obj \
	$O\evp/m_sha1.obj \
	$O\evp/e_rc4.obj \
	$O\evp/m_null.obj \
	$O\evp/e_null.obj \
	$O\evp/pmeth_fn.obj \
	$O\evp/m_wp.obj \
	$O\evp/p_verify.obj \
	$O\evp/digest.obj \
	$O\evp/m_gost2814789.obj \
	$O\evp/m_streebog.obj \
	$O\evp/evp_lib.obj \
	$O\evp/e_xcbc_d.obj \
	$O\evp/e_camellia.obj \
	$O\engine/eng_cnf.obj \
	$O\engine/tb_dsa.obj \
	$O\engine/eng_fat.obj \
	$O\engine/tb_digest.obj \
	$O\engine/eng_ctrl.obj \
	$O\engine/tb_pkmeth.obj \
	$O\engine/tb_asnmth.obj \
	$O\engine/tb_cipher.obj \
	$O\engine/eng_list.obj \
	$O\engine/tb_store.obj \
	$O\engine/eng_openssl.obj \
	$O\engine/eng_all.obj \
	$O\engine/eng_err.obj \
	$O\engine/eng_lib.obj \
	$O\engine/eng_table.obj \
	$O\engine/tb_ecdsa.obj \
	$O\engine/tb_ecdh.obj \
	$O\engine/eng_dyn.obj \
	$O\engine/eng_init.obj \
	$O\engine/tb_dh.obj \
	$O\engine/eng_pkey.obj \
	$O\engine/tb_rsa.obj \
	$O\engine/tb_rand.obj \
	$O\objects/obj_xref.obj \
	$O\objects/obj_dat.obj \
	$O\objects/obj_err.obj \
	$O\objects/obj_lib.obj \
	$O\objects/o_names.obj \
	$O\o_str.obj \
	$O\pkcs12/p12_p8d.obj \
	$O\pkcs12/p12_attr.obj \
	$O\pkcs12/p12_asn.obj \
	$O\pkcs12/p12_mutl.obj \
	$O\pkcs12/p12_crt.obj \
	$O\pkcs12/p12_init.obj \
	$O\pkcs12/p12_npas.obj \
	$O\pkcs12/p12_key.obj \
	$O\pkcs12/p12_add.obj \
	$O\pkcs12/p12_p8e.obj \
	$O\pkcs12/p12_utl.obj \
	$O\pkcs12/p12_kiss.obj \
	$O\pkcs12/p12_crpt.obj \
	$O\pkcs12/p12_decr.obj \
	$O\pkcs12/pk12err.obj \
	$O\aes/aes_ige.obj \
	$O\aes/aes_ofb.obj \
	$O\aes/aes_cbc.obj \
	$O\aes/aes_cfb.obj \
	$O\aes/aes_wrap.obj \
	$O\aes/aes_ecb.obj \
	$O\aes/aes_core.obj \
	$O\aes/aes_ctr.obj \
	$O\aes/aes_misc.obj \
	$O\cast/c_ecb.obj \
	$O\cast/c_enc.obj \
	$O\cast/c_ofb64.obj \
	$O\cast/c_skey.obj \
	$O\cast/c_cfb64.obj \
	$O\ec/ec_ameth.obj \
	$O\ec/ec2_smpl.obj \
	$O\ec/ec_oct.obj \
	$O\ec/ec_err.obj \
	$O\ec/ec_cvt.obj \
	$O\ec/ecp_smpl.obj \
	$O\ec/ec_lib.obj \
	$O\ec/ec_mult.obj \
	$O\ec/eck_prn.obj \
	$O\ec/ec_check.obj \
	$O\ec/ec_key.obj \
	$O\ec/ecp_mont.obj \
	$O\ec/ec2_oct.obj \
	$O\ec/ec_pmeth.obj \
	$O\ec/ec2_mult.obj \
	$O\ec/ec_curve.obj \
	$O\ec/ec_asn1.obj \
	$O\ec/ec_print.obj \
	$O\ec/ecp_nist.obj \
	$O\ec/ecp_oct.obj \
	$O\compat/explicit_bzero_win.obj \
	$O\compat/getentropy_win.obj \
	$O\compat/getpagesize.obj \
	$O\compat/strcasecmp.obj \
	$O\compat/arc4random_uniform.obj \
	$O\compat/arc4random.obj \
	$O\compat/posix_win.obj \
	$O\bio/b_win.obj \
	$O\ui/ui_openssl_win.obj \
	$O\dll_main.obj \

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
all: $O\ts $O\dsa $O\chacha $O\ecdsa $O\rc2 $O\whrlpool $O\x509v3 $O\bf $O\gost $O\idea $O\buffer $O\modes $O\rra_portable $O\md4 $O\dso $O\hkdf $O\stack $O\txt_db $O\bn $O\dh $O\ripemd $O\conf $O\ecdh $O\err $O\pkcs7 $O\comp $O\lhash $O\hmac $O\rand $O\ocsp $O\x509 $O\ui $O\poly1305 $O\rc4 $O\bio $O\sha $O\des $O\camellia $O\rsa $O\compat $O\cmac $O\asn1 $O\curve25519 $O\md5 $O\pem $O\evp $O\engine $O\objects $O\pkcs12 $O\aes $O\cast $O\ec $O $(DLIB_FILE0) $(SLIB_FILE0) 

# Mkdir rule.
$O\ts:
	if not exist $O\ts mkdir $O\ts

$O\dsa:
	if not exist $O\dsa mkdir $O\dsa

$O\chacha:
	if not exist $O\chacha mkdir $O\chacha

$O\ecdsa:
	if not exist $O\ecdsa mkdir $O\ecdsa

$O\rc2:
	if not exist $O\rc2 mkdir $O\rc2

$O\whrlpool:
	if not exist $O\whrlpool mkdir $O\whrlpool

$O\x509v3:
	if not exist $O\x509v3 mkdir $O\x509v3

$O\bf:
	if not exist $O\bf mkdir $O\bf

$O\gost:
	if not exist $O\gost mkdir $O\gost

$O\idea:
	if not exist $O\idea mkdir $O\idea

$O\buffer:
	if not exist $O\buffer mkdir $O\buffer

$O\modes:
	if not exist $O\modes mkdir $O\modes

$O\rra_portable:
	if not exist $O\rra_portable mkdir $O\rra_portable

$O\md4:
	if not exist $O\md4 mkdir $O\md4

$O\dso:
	if not exist $O\dso mkdir $O\dso

$O\hkdf:
	if not exist $O\hkdf mkdir $O\hkdf

$O\stack:
	if not exist $O\stack mkdir $O\stack

$O\txt_db:
	if not exist $O\txt_db mkdir $O\txt_db

$O\bn:
	if not exist $O\bn mkdir $O\bn

$O\dh:
	if not exist $O\dh mkdir $O\dh

$O\ripemd:
	if not exist $O\ripemd mkdir $O\ripemd

$O\conf:
	if not exist $O\conf mkdir $O\conf

$O\ecdh:
	if not exist $O\ecdh mkdir $O\ecdh

$O\err:
	if not exist $O\err mkdir $O\err

$O\pkcs7:
	if not exist $O\pkcs7 mkdir $O\pkcs7

$O\comp:
	if not exist $O\comp mkdir $O\comp

$O\lhash:
	if not exist $O\lhash mkdir $O\lhash

$O\hmac:
	if not exist $O\hmac mkdir $O\hmac

$O\rand:
	if not exist $O\rand mkdir $O\rand

$O\ocsp:
	if not exist $O\ocsp mkdir $O\ocsp

$O\x509:
	if not exist $O\x509 mkdir $O\x509

$O\ui:
	if not exist $O\ui mkdir $O\ui

$O\poly1305:
	if not exist $O\poly1305 mkdir $O\poly1305

$O\rc4:
	if not exist $O\rc4 mkdir $O\rc4

$O\bio:
	if not exist $O\bio mkdir $O\bio

$O\sha:
	if not exist $O\sha mkdir $O\sha

$O\des:
	if not exist $O\des mkdir $O\des

$O\camellia:
	if not exist $O\camellia mkdir $O\camellia

$O\rsa:
	if not exist $O\rsa mkdir $O\rsa

$O\compat:
	if not exist $O\compat mkdir $O\compat

$O\cmac:
	if not exist $O\cmac mkdir $O\cmac

$O\asn1:
	if not exist $O\asn1 mkdir $O\asn1

$O\curve25519:
	if not exist $O\curve25519 mkdir $O\curve25519

$O\md5:
	if not exist $O\md5 mkdir $O\md5

$O\pem:
	if not exist $O\pem mkdir $O\pem

$O\evp:
	if not exist $O\evp mkdir $O\evp

$O\engine:
	if not exist $O\engine mkdir $O\engine

$O\objects:
	if not exist $O\objects mkdir $O\objects

$O\pkcs12:
	if not exist $O\pkcs12 mkdir $O\pkcs12

$O\aes:
	if not exist $O\aes mkdir $O\aes

$O\cast:
	if not exist $O\cast mkdir $O\cast

$O\ec:
	if not exist $O\ec mkdir $O\ec

$O:
	if not exist $O mkdir $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	@echo LIB /NOLOGO /OUT:$(SLIB_FILE0) {[objs]} $(SUB_LIBS)
	@     LIB /NOLOGO /OUT:$(SLIB_FILE0) $(OBJS0) $(SUB_LIBS)

gsl.myf: $(SLIB_FILE0)
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE)

gsl.def: gsl.myf
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -d gsl.myf gsl.def

$(DLIB_FILE0): $(OBJS0) $(SLIB_FILE0) gsl.def
	@echo $(LINKER) /DLL /OUT:$(DLIB_FILE0) /IMPLIB:$(ILIB_FILE0) {[objs]} $(SUB_LIBS)  ws2_32.lib advapi32.lib  /DEF:gsl.def
	@     $(LINKER) /DLL /OUT:$(DLIB_FILE0) /IMPLIB:$(ILIB_FILE0) $(OBJS0) $(SUB_LIBS)  ws2_32.lib advapi32.lib  /DEF:gsl.def


# Suffix rule.
{$S\ts}.c{$O\ts}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ts}.cpp{$O\ts}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\dsa}.c{$O\dsa}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\dsa}.cpp{$O\dsa}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\chacha}.c{$O\chacha}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\chacha}.cpp{$O\chacha}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ecdsa}.c{$O\ecdsa}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ecdsa}.cpp{$O\ecdsa}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\rc2}.c{$O\rc2}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\rc2}.cpp{$O\rc2}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\whrlpool}.c{$O\whrlpool}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\whrlpool}.cpp{$O\whrlpool}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\x509v3}.c{$O\x509v3}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\x509v3}.cpp{$O\x509v3}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\bf}.c{$O\bf}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\bf}.cpp{$O\bf}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\gost}.c{$O\gost}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\gost}.cpp{$O\gost}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\idea}.c{$O\idea}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\idea}.cpp{$O\idea}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\buffer}.c{$O\buffer}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\buffer}.cpp{$O\buffer}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\modes}.c{$O\modes}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\modes}.cpp{$O\modes}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\rra_portable}.c{$O\rra_portable}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\rra_portable}.cpp{$O\rra_portable}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\md4}.c{$O\md4}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\md4}.cpp{$O\md4}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\dso}.c{$O\dso}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\dso}.cpp{$O\dso}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\hkdf}.c{$O\hkdf}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\hkdf}.cpp{$O\hkdf}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\stack}.c{$O\stack}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\stack}.cpp{$O\stack}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\txt_db}.c{$O\txt_db}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\txt_db}.cpp{$O\txt_db}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\bn}.c{$O\bn}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\bn}.cpp{$O\bn}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\dh}.c{$O\dh}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\dh}.cpp{$O\dh}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ripemd}.c{$O\ripemd}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ripemd}.cpp{$O\ripemd}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\conf}.c{$O\conf}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\conf}.cpp{$O\conf}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ecdh}.c{$O\ecdh}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ecdh}.cpp{$O\ecdh}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\err}.c{$O\err}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\err}.cpp{$O\err}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\pkcs7}.c{$O\pkcs7}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\pkcs7}.cpp{$O\pkcs7}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\comp}.c{$O\comp}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\comp}.cpp{$O\comp}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\lhash}.c{$O\lhash}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\lhash}.cpp{$O\lhash}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\hmac}.c{$O\hmac}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\hmac}.cpp{$O\hmac}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\rand}.c{$O\rand}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\rand}.cpp{$O\rand}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ocsp}.c{$O\ocsp}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ocsp}.cpp{$O\ocsp}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\x509}.c{$O\x509}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\x509}.cpp{$O\x509}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ui}.c{$O\ui}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ui}.cpp{$O\ui}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\poly1305}.c{$O\poly1305}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\poly1305}.cpp{$O\poly1305}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\rc4}.c{$O\rc4}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\rc4}.cpp{$O\rc4}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\bio}.c{$O\bio}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\bio}.cpp{$O\bio}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\sha}.c{$O\sha}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\sha}.cpp{$O\sha}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\des}.c{$O\des}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\des}.cpp{$O\des}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\camellia}.c{$O\camellia}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\camellia}.cpp{$O\camellia}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\rsa}.c{$O\rsa}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\rsa}.cpp{$O\rsa}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\compat}.c{$O\compat}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\compat}.cpp{$O\compat}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\cmac}.c{$O\cmac}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\cmac}.cpp{$O\cmac}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\asn1}.c{$O\asn1}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\asn1}.cpp{$O\asn1}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\curve25519}.c{$O\curve25519}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\curve25519}.cpp{$O\curve25519}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\md5}.c{$O\md5}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\md5}.cpp{$O\md5}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\pem}.c{$O\pem}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\pem}.cpp{$O\pem}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\evp}.c{$O\evp}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\evp}.cpp{$O\evp}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\engine}.c{$O\engine}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\engine}.cpp{$O\engine}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\objects}.c{$O\objects}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\objects}.cpp{$O\objects}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\pkcs12}.c{$O\pkcs12}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\pkcs12}.cpp{$O\pkcs12}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\aes}.c{$O\aes}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\aes}.cpp{$O\aes}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\cast}.c{$O\cast}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\cast}.cpp{$O\cast}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ec}.c{$O\ec}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\ec}.cpp{$O\ec}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S}.c{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S}.cpp{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:

# Install exec rule.
install_exec:
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" $(MY_LIBS_ROOT)\$(DLIBS_DIR)\ $(CP_END)

# Install dlib rule.
install_dlib: $(DLIB_FILE0)
	@if not exist $(MY_LIBS_ROOT)\$(DLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(DLIBS_DIR) 
	@if exist "$(DLIB_FILE0)" @$(CP) /F "$(DLIB_FILE0)" $(MY_LIBS_ROOT)\$(DLIBS_DIR)\ $(CP_END)

# Install slib rule.
install_slib: $(SLIB_FILE0)
	@if not exist $(MY_LIBS_ROOT)\$(SLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(SLIBS_DIR) 
	@if exist "$(SLIB_FILE0)" @$(CP) /F "$(SLIB_FILE0)" $(MY_LIBS_ROOT)\$(SLIBS_DIR)\ $(CP_END)

# Install rule.
install: all install_slib install_dlib 


# Clean rule.
clean:
	rmdir /S /Q $O\ 

# Src and Headers Dependency
whrlpool/wp_block.obj: x86_arch.h
gost/gostr341194.obj: md32_common.h
gost/gost2814789.obj: md32_common.h
o_time.obj: o_time.h
modes/gcm128.obj: x86_arch.h
rra_portable/inet_aton.obj: rra_portable/inet_conv.h
rra_portable/getnameinfo.obj: rra_portable/getnameinfo.h rra_portable/socket.h rra_portable/snprintf.h
rra_portable/snprintf.obj: rra_portable/snprintf.h
rra_portable/inet_ntoa.obj: rra_portable/inet_conv.h rra_portable/snprintf.h
rra_portable/getaddrinfo.obj: rra_portable/getaddrinfo.h rra_portable/inet_conv.h
rra_portable/inet_ntop.obj: rra_portable/inet_conv.h rra_portable/snprintf.h rra_portable/socket.h
bn/bn_exp.obj: constant_time_locl.h
x509/x509_def.obj: cryptlib.h
crypto_init.obj: cryptlib.h
cversion.obj: cryptlib.h
bio/b_sock.obj: rra_portable/getaddrinfo.h rra_portable/getnameinfo.h
sha/sha256.obj: md32_common.h
asn1/a_time_tm.obj: o_time.h
asn1/a_time.obj: o_time.h
evp/e_aes.obj: x86_arch.h
evp/c_all.obj: cryptlib.h
evp/e_rc4_hmac_md5.obj: x86_arch.h
evp/e_aes_cbc_hmac_sha1.obj: constant_time_locl.h x86_arch.h
engine/eng_list.obj: cryptlib.h
engine/eng_all.obj: cryptlib.h
