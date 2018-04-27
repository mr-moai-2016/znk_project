#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_str.h>
#include <Znk_str_ary.h>
#include <Znk_myf.h>
#include <Znk_bfr.h>
#include <Znk_vpod.h>
#include <stdio.h>
#include <assert.h>

/* from WinNT.h IMAGE_ARCHIVE_MEMBER_HEADER */
typedef struct {
	uint8_t Name[16];                          // File member name - `/' terminated.
	uint8_t Date[12];                          // File member date - decimal.
	uint8_t UserID[6];                         // File member user id - decimal.
	uint8_t GroupID[6];                        // File member group id - decimal.
	uint8_t Mode[8];                           // File member mode - octal.
	uint8_t Size[10];                          // File member size - decimal.
	uint8_t EndHeader[2];                      // String to end header.
} ImageArchiveMemberHeader;

/* from WinNT.h IMAGE_FILE_HEADER */
typedef struct {
	uint16_t Machine;
	uint16_t NumberOfSections;
	uint32_t TimeDateStamp;
	uint32_t PointerToSymbolTable;
	uint32_t NumberOfSymbols;
	uint16_t SizeOfOptionalHeader;
	uint16_t Characteristics;
} ImageFileHeader;

/* from WinNT.h */
#ifndef   IMAGE_ARCHIVE_LINKER_MEMBER

#  define IMAGE_ARCHIVE_LINKER_MEMBER          "/               "
#  define IMAGE_ARCHIVE_LONGNAMES_MEMBER       "//              "

#  define IMAGE_SIZEOF_SHORT_NAME 8
#  define IMAGE_SIZEOF_SYMBOL 18
#  define IMAGE_SIZEOF_RELOCATION 10

#  define IMAGE_SCN_ALIGN_1BYTES               0x00100000  //
#  define IMAGE_SCN_ALIGN_2BYTES               0x00200000  //
#  define IMAGE_SCN_ALIGN_4BYTES               0x00300000  //
#  define IMAGE_SCN_ALIGN_8BYTES               0x00400000  //
#  define IMAGE_SCN_ALIGN_16BYTES              0x00500000  // Default alignment if no others are specified.
#  define IMAGE_SCN_ALIGN_32BYTES              0x00600000  //
#  define IMAGE_SCN_ALIGN_64BYTES              0x00700000  //
#  define IMAGE_SCN_ALIGN_128BYTES             0x00800000  //
#  define IMAGE_SCN_ALIGN_256BYTES             0x00900000  //
#  define IMAGE_SCN_ALIGN_512BYTES             0x00A00000  //
#  define IMAGE_SCN_ALIGN_1024BYTES            0x00B00000  //
#  define IMAGE_SCN_ALIGN_2048BYTES            0x00C00000  //
#  define IMAGE_SCN_ALIGN_4096BYTES            0x00D00000  //
#  define IMAGE_SCN_ALIGN_8192BYTES            0x00E00000  //

#  define IMAGE_SYM_DTYPE_FUNCTION 2

#  define IMAGE_SYM_CLASS_EXTERNAL            0x0002
#  define IMAGE_SYM_CLASS_EXTERNAL_DEF        0x0005

#  define IMAGE_SYM_UNDEFINED           (int16_t)0        // Symbol is undefined or is common.
#  define IMAGE_SYM_ABSOLUTE            (int16_t)-1       // Symbol is an absolute value.
#  define IMAGE_SYM_DEBUG               (int16_t)-2       // Symbol is a special debug item.
#  define IMAGE_SYM_SECTION_MAX         0xFEFF            // Values 0xFF00-0xFFFF are special

#  define IMAGE_FILE_MACHINE_I386              0x014c  // Intel 386.
#  define IMAGE_FILE_MACHINE_AMD64             0x8664  // AMD64 (K8)

#endif

/* from WinNT.h IMAGE_SECTION_HEADER */
typedef struct {
	uint8_t Name[ IMAGE_SIZEOF_SHORT_NAME ];
	union {
		uint32_t PhysicalAddress;
		uint32_t VirtualSize;
	} Misc;
	uint32_t VirtualAddress;
	uint32_t SizeOfRawData;
	uint32_t PointerToRawData;
	uint32_t PointerToRelocations;
	uint32_t PointerToLinenumbers;
	uint16_t NumberOfRelocations;
	uint16_t NumberOfLinenumbers;
	uint32_t Characteristics;
} ImageSectionHeader;

/* from WinNT.h IMAGE_SYMBOL */
typedef struct {
	union {
		uint8_t ShortName[8];
		struct {
			uint32_t Short;     // if 0, use LongName
			uint32_t Long;      // offset into string table
		} Name;
		uint32_t LongName[2];    // PBYTE [2]
	} N;
	uint32_t Value;
	int16_t  SectionNumber;
	uint16_t Type;
	uint8_t  StorageClass;
	uint8_t  NumberOfAuxSymbols;
} ImageSymbol;

/* from WinNT.h IMAGE_RELOCATION */
typedef struct {
	union {
		uint32_t VirtualAddress;
		uint32_t RelocCount;             // Set to the real count when IMAGE_SCN_LNK_NRELOC_OVFL is set
	} u;
	uint32_t SymbolTableIndex;
	uint16_t Type;
} ImageRelocation;

static size_t
readImageArchiveMemberHeader( ImageArchiveMemberHeader* iamh, ZnkFile fp, bool dump )
{
	size_t read_size = Znk_fread( (uint8_t*)iamh, 60, fp );

	if( read_size == 60 && dump ){
		char buf[ 256 ] = "";
		assert( iamh->Name[0] != 0x0a );
		ZnkS_copy( buf, sizeof(buf), (char*)iamh->Name, sizeof(iamh->Name) ); 
		Znk_printf_e( "Name=[%s]\n", buf );
		ZnkS_copy( buf, sizeof(buf), (char*)iamh->Date, sizeof(iamh->Date) ); 
		Znk_printf_e( "Date=[%s]\n", buf );
		ZnkS_copy( buf, sizeof(buf), (char*)iamh->UserID, sizeof(iamh->UserID) ); 
		Znk_printf_e( "UserID=[%s]\n", buf );
		ZnkS_copy( buf, sizeof(buf), (char*)iamh->GroupID, sizeof(iamh->GroupID) ); 
		Znk_printf_e( "GroupID=[%s]\n", buf );
		ZnkS_copy( buf, sizeof(buf), (char*)iamh->Mode, sizeof(iamh->Mode) ); 
		Znk_printf_e( "Mode=[%s]\n", buf );
		ZnkS_copy( buf, sizeof(buf), (char*)iamh->Size, sizeof(iamh->Size) ); 
		Znk_printf_e( "Size=[%s]\n", buf );
		ZnkS_copy( buf, sizeof(buf), (char*)iamh->EndHeader, sizeof(iamh->EndHeader) ); 
		Znk_printf_e( "EndHeader=[%s]\n", buf );
	}

	return read_size;
}

static size_t
getIAMHSize( const ImageArchiveMemberHeader* iamh )
{
	char buf[ 256 ] = "";
	size_t size = 0;
	ZnkS_copy( buf, sizeof(buf), (char*)iamh->Size, sizeof(iamh->Size) ); 
	ZnkS_getSzU( &size, buf );
	return size;
}

static bool
readCoffObjMember( ZnkStrAry export_symbols, ImageArchiveMemberHeader* iamh, ZnkFile fp,
		ZnkBfr long_name_body, bool skip_hdr, bool debug )
{
	ImageFileHeader    ifh = { 0 };
	ImageSectionHeader ish = {{ 0 }};
	ImageSymbol        is  = {{{ 0 }}};
	size_t   img_size = 0;
	ZnkStr   obj_name = ZnkStr_new( "" );
	size_t   sec_idx;
	size_t   stb_idx;
	size_t   is_idx;
	ZnkBfr   rdsz_list    = ZnkBfr_create_null();
	ZnkBfr   strtbl       = ZnkBfr_create_null();
	ZnkBfr   symname_list = ZnkBfr_create_null();
	ZnkBfr   is_list      = ZnkBfr_create_null();
	ZnkBfr   img          = ZnkBfr_create_null();
	char     name_buf[ 16 ] = "";
	size_t   aux_count    = 0;
	size_t   is_list_size = 0;
	uint32_t strtbl_size  = 0;
	size_t   read_size    = 0;
	bool     result       = false;
	size_t   img_pos      = 0;
	size_t   symtbl_size  = 0;
	bool     is_bar       = false;

	if( !skip_hdr ){
		read_size = readImageArchiveMemberHeader( iamh, fp, debug );
		if( read_size != 60 ){
			if( Znk_feof( fp ) ){
				result = true;
				goto FUNC_END;
			}
			Znk_printf_e( "Error : readImageArchiveMemberHeader read_size=[%zu]\n", read_size );
			goto FUNC_END;
		}
	}
	img_size = getIAMHSize( iamh );
	if( img_size % 2 ){ ++img_size; } /* for 2byte padding */

	if( debug ){
		Znk_printf_e( "\n" );
		Znk_printf_e( "img_size(iamh)=[%zu]\n", img_size );
	}

	if( iamh->Name[0] == '/' && ZnkBfr_size(long_name_body) ){
		char buf[ 256 ] = "";
		size_t pos = 0;
		size_t idx;
		const size_t long_name_body_size = ZnkBfr_size(long_name_body);
		const uint8_t* long_name_data;
		ZnkS_copy( buf, sizeof(buf), (char*)iamh->Name+1, sizeof(iamh->Name)-1 ); 
		ZnkS_getSzU( &pos, buf );
		long_name_data = ZnkBfr_data(long_name_body);
		for( idx=pos; idx<long_name_body_size; ++idx ){
			/* VC libではNUL文字で、UNIX arファイルでは / 0x0a で区切られている */
			if( long_name_data[ idx ] == '\0' || 
			    ( long_name_data[ idx ] == '/' && long_name_data[ idx+1 ] == 0x0a ) )
			{
				break;
			}
			ZnkStr_add_c( obj_name, long_name_data[idx] );
		}
	} else {
		size_t idx;
		ZnkStr_clear( obj_name );
		for( idx=0; idx<16; ++idx ){
			if( iamh->Name[idx] == '/' ){
				break;
			} else {
				ZnkStr_add_c( obj_name, iamh->Name[idx] );
			}
		}
	}

	if( ZnkStr_isEnd( obj_name, ".a" ) && img_size >= 8 ){
		uint8_t buf[ 16 ] = { 0 };
		/* sub-archive */
		/* とりあえずこの内部のシンボルを拾うことはないため、
		 * 単に無視すればよい */
		Znk_printf_e( "Report : sub-archive [%s] is detected.\n", ZnkStr_cstr(obj_name) );
		read_size = Znk_fread( buf, 8, fp );
		if( read_size == 8 ){
			/* 念のため !<arch>\n をチェック. */
			if( Znk_memcmp( buf, "!<arch>\n", 8 ) == 0 ){
				Znk_fseek( fp, img_size-8, SEEK_CUR );
				result = true;
				goto FUNC_END;
			}
		}
		Znk_printf_e( "Error : cannot read archive member [%s]. This member seems to have sub-archive, but !<arch> is not found.\n",
				ZnkStr_cstr(obj_name) );
		goto FUNC_END;
	}

	ZnkBfr_resize( img, img_size );

	read_size = Znk_fread( ZnkBfr_data(img), img_size, fp );
	if( read_size != img_size ){
		Znk_printf_e( "Error : cannot read archive member [%s].( menber img_size=[%zu] but read_size=[%zu] )\n",
				ZnkStr_cstr(obj_name), img_size, read_size );
		goto FUNC_END;
	} else {
		if( debug ){
			Znk_printf_e( "OK : read archive member [%s] image.\n", ZnkStr_cstr(obj_name) );
		}
	}

	Znk_memcpy( &ifh, ZnkBfr_data(img)+img_pos, sizeof(ifh) );
	img_pos += sizeof(ifh);
	if( debug ){
		Znk_printf_e( "Machine=[%04x]\n",              ifh.Machine );
		Znk_printf_e( "NumberOfSections=[%u]\n",       ifh.NumberOfSections );
		Znk_printf_e( "TimeDateStamp=[%08x]\n",        ifh.TimeDateStamp );
		Znk_printf_e( "PointerToSymbolTable=[%08x]\n", ifh.PointerToSymbolTable );
		Znk_printf_e( "NumberOfSymbols=[%I32u]\n",     ifh.NumberOfSymbols );
		Znk_printf_e( "SizeOfOptionalHeader=[%u]\n",   ifh.SizeOfOptionalHeader );
		Znk_printf_e( "Characteristics=[%04x]\n\n",    ifh.Characteristics );
	}
	assert( img_size >= sizeof(ifh) );
	if( ifh.Machine == IMAGE_FILE_MACHINE_I386 ){
		is_bar = true;
	} else {
		is_bar = false;
	}

	ZnkBfr_reserve( rdsz_list, ifh.NumberOfSections*sizeof(uint32_t) );

	/* SectionHeader list */
	for( sec_idx=0; sec_idx<ifh.NumberOfSections; ++sec_idx ){
		Znk_memcpy( &ish, ZnkBfr_data(img)+img_pos, sizeof(ish) );
		img_pos += sizeof(ish);

		assert( img_size >= sizeof(ish) );
		if( debug ){
			char buf[ 256 ] = "";
			ZnkS_copy( buf, sizeof(buf), (char*)ish.Name, sizeof(ish.Name) ); 
			Znk_printf_e( "Section : Name=[%s]\n", buf );
			Znk_printf_e( "  VirtualSize=[%I32u]\n",         ish.Misc.VirtualSize );
			Znk_printf_e( "  VirtualAddress=[%08x]\n",       ish.VirtualAddress );
			Znk_printf_e( "  SizeOfRawData=[%I32u]\n",       ish.SizeOfRawData );
			Znk_printf_e( "  PointerToRawData=[%08x]\n",     ish.PointerToRawData );
			Znk_printf_e( "  NumberOfRelocations=[%u]\n",    ish.NumberOfRelocations );
			Znk_printf_e( "  NumberOfLinenumbers=[%u]\n",    ish.NumberOfLinenumbers );
			Znk_printf_e( "  Characteristics=[%08x]\n",      ish.Characteristics );
		}
		ZnkVUInt32_push_bk( rdsz_list, ish.SizeOfRawData + ish.NumberOfRelocations*IMAGE_SIZEOF_RELOCATION );
	}

	/***
	 * RawDataを解析するには必ずPointerToRawDataを基点にしなければならない.
	 * この基点は必ずしも順番に並んでいるとは限らないし、必ずしもSectionHeaderより後ろにあるとも限らない.
	 * 単なるサイズの累積和で位置をインクリメントするのは無意味である.
	 */

	/* SymbolTable */
	img_pos = ifh.PointerToSymbolTable;
	if( debug ){
		Znk_printf_e( "Report : ifh.PointerToSymbolTable=[%08x]\n", ifh.PointerToSymbolTable );
	}
	symtbl_size = img_size - img_pos;
	for( stb_idx=0; stb_idx<ifh.NumberOfSymbols; ++stb_idx ){
		Znk_memcpy( &is, ZnkBfr_data(img)+img_pos, IMAGE_SIZEOF_SYMBOL );
		img_pos += IMAGE_SIZEOF_SYMBOL;

		assert( symtbl_size >= IMAGE_SIZEOF_SYMBOL );
		symtbl_size -= IMAGE_SIZEOF_SYMBOL;
		if( aux_count == 0 ){
			ZnkBfr_append_dfr( is_list, (uint8_t*)&is, IMAGE_SIZEOF_SYMBOL );
			aux_count = is.NumberOfAuxSymbols;
			if( debug ){
				Znk_printf_e( "is : Value=[%I32u] SectionNumber=[%d] Type=[%u] StorageClass=[%02x] NumberOfAuxSymbols=[%02x]\n",
						is.Value, is.SectionNumber, is.Type, is.StorageClass, is.NumberOfAuxSymbols );
			}
		} else {
			/* skip */
			--aux_count;
		}
	}

	Znk_fflush( Znk_stderr() );
	/* StringTable */
	{
		Znk_memcpy( &strtbl_size, ZnkBfr_data(img)+img_pos, sizeof(uint32_t) );
		img_pos += sizeof(uint32_t);

		Znk_fflush( Znk_stderr() );
		ZnkBfr_reserve( strtbl, strtbl_size );
		ZnkBfr_push_bk_32( strtbl, strtbl_size, true );
		if( strtbl_size > symtbl_size ){
			Znk_printf_e( "Error : invalid strtbl_size=[%I32u] dmp=[%08x]\n", strtbl_size, strtbl_size );
			goto FUNC_END;
		}

		Znk_memcpy( ZnkBfr_data(strtbl)+sizeof(uint32_t), ZnkBfr_data(img)+img_pos, strtbl_size-sizeof(uint32_t) );
		img_pos += (strtbl_size-sizeof(uint32_t));

	}

	/* Dump */
	is_list_size = ZnkBfr_size( is_list ) / IMAGE_SIZEOF_SYMBOL;
	for( is_idx=0; is_idx<is_list_size; ++is_idx ){
		char* name="?";
		Znk_memcpy( &is, ZnkBfr_data( is_list ) + is_idx * IMAGE_SIZEOF_SYMBOL, IMAGE_SIZEOF_SYMBOL );
		if( is.N.Name.Short == 0 ){
			name = (char*)ZnkBfr_data( strtbl ) + is.N.Name.Long;
		} else {
			ZnkS_copy( name_buf, sizeof(name_buf), (char*)is.N.ShortName, 8 );
			name = name_buf;
		}
		if( debug ){
			Znk_printf_e( "SymTbl : name=[%s]\n", name );
			Znk_printf_e( "       : Value=[%08x]\n", is.Value );
			Znk_printf_e( "       : SectionNumber=[%d]\n", is.SectionNumber );
			Znk_printf_e( "       : Type=[%04x]\n", is.Type );
			Znk_printf_e( "       : StorageClass=[%02x]\n", is.StorageClass );
			Znk_printf_e( "       : NumberOfAuxSymbols=[%02x]\n", is.NumberOfAuxSymbols );
		}
		if( is.Type == 0x20 && is.StorageClass == IMAGE_SYM_CLASS_EXTERNAL && is.SectionNumber != IMAGE_SYM_UNDEFINED ){
			if( is_bar ){
				ZnkStrAry_push_bk_cstr( export_symbols, name+1, Znk_NPOS );
			} else {
				ZnkStrAry_push_bk_cstr( export_symbols, name, Znk_NPOS );
			}
		}
	}

	result = true;

FUNC_END:
	if( !result ){
		ZnkFile f2 = Znk_fopen( "member_dump.dat", "wb" );
		Znk_fwrite( ZnkBfr_data(img), ZnkBfr_size(img), f2 );
		Znk_fclose( f2 );
	}

	ZnkStr_delete( obj_name );
	ZnkBfr_destroy( rdsz_list );
	ZnkBfr_destroy( strtbl );
	ZnkBfr_destroy( symname_list );
	ZnkBfr_destroy( is_list );
	ZnkBfr_destroy( img );

	return result;
}

static bool
readSymnameList( size_t num_of_symbols, ZnkStr symname, ZnkFile fp, size_t read_member_size )
{
	size_t idx;
	int ch;
	for( idx=0; idx<num_of_symbols; ++idx ){
		ZnkStr_clear( symname );
		while( true ){
			ch = Znk_fgetc( fp );
			++read_member_size;
			if( ch == EOF ){
				Znk_printf_e( "Error : Unexpected EOF\n" );
				return false;
			}
			if( ch == '\0' ){
				break;
			}
			ZnkStr_add_c( symname, (char)ch );
		}
		//Znk_printf_e( "symname=[%s]\n", ZnkStr_cstr(symname) );
	}
	/* read 0x0a or 0x00 padding */
	//Znk_printf_e( "read_member_size=[%zu]\n", read_member_size );
	if( read_member_size % 2 ){
		ch = Znk_fgetc( fp );
		++read_member_size;
		/***
		 * 通常padding文字には0x0aが使われるが、
		 * MinGW64のaライブラリでは0x00でpaddingされている.
		 */
		if( ch != 0x0a && ch != 0x00 ){
			Znk_printf_e( "Error : padding ch is not 0x0a and 0x00. [%02x]\n", ch );
			return false;
		}
	}
	return true;
}

bool
ZnkLibaScan_getExportFunctionSymbols( const char* slib_filename, ZnkStrAry export_symbols, bool debug )
{
	ImageArchiveMemberHeader iamh = {{ 0 }};
	ZnkFile       fp = NULL;
	uint32_t      num_of_symbols = 0;
	ZnkStr        symname        = NULL;
	ZnkBfr        long_name_body = NULL;
	ZnkStr        obj_name       = NULL;
	bool          skip_hdr       = false;
	uint32_t      num_of_members = 0;
	size_t        mem_idx;
	size_t        read_size = 0;
	size_t        read_member_size = 0;
	bool          result = false;

	fp = Znk_fopen( slib_filename, "rb" );
	if( fp == NULL ){
		goto FUNC_END;
	}

	symname        = ZnkStr_new( "" );
	long_name_body = ZnkBfr_create_null();
	obj_name       = ZnkStr_new( "" );
	//myf            = ZnkMyf_create();

	/* read IMAGE_ARCHIVE_START */
	{
		uint8_t buf[ 256 ] = { 0 };
		read_size = Znk_fread( buf, 8, fp );
		if( !ZnkS_eq( (char*)buf, "!<arch>\n" ) ){
			/* error */
			Znk_printf_e( "Error : This file does not begin !<arch>.\n" );
			goto FUNC_END;
		}
	}

	/* First Linker Member(for compatiblity) */
	readImageArchiveMemberHeader( &iamh, fp, false );
	/* big endian */
	read_size = Znk_fread_blk( (uint8_t*)&num_of_symbols, sizeof(num_of_symbols), 1, fp );
	if( read_size != 1 ){
		/* error */
		Znk_printf_e( "Error : Cannot read first num_of_symbols\n" );
		goto FUNC_END;
	}
	read_member_size += sizeof(num_of_symbols);
	Znk_swap4byte( (uint8_t*)&num_of_symbols );
	//Znk_printf_e( "num_of_symbols=[%I32u]\n", num_of_symbols );
	{
		size_t idx;
		uint32_t offset = 0;
		for( idx=0; idx<num_of_symbols; ++idx ){
			read_size = Znk_fread_blk( (uint8_t*)&offset, sizeof(offset), 1, fp );
			if( read_size != 1 ){
				/* error */
				Znk_printf_e( "Error : Cannot read [%zu/%I32u]-th offset\n", idx, num_of_symbols );
				goto FUNC_END;
			}
			read_member_size += sizeof(offset);
			Znk_swap4byte( (uint8_t*)&offset );
			//Znk_printf_e( "offset=[%I32u]\n", offset );
		}
	}
	if( !readSymnameList( num_of_symbols, symname, fp, read_member_size ) ){
		/* error */
		goto FUNC_END;
	}

	/* Second Linker Member */
	read_member_size = 0;
	readImageArchiveMemberHeader( &iamh, fp, debug );
	if( Znk_memcmp( iamh.Name, (uint8_t*)IMAGE_ARCHIVE_LINKER_MEMBER, 16 ) == 0 ){
		/***
		 * 2番目のMemberもIMAGE_ARCHIVE_LINKER_MEMBERで始まる.
		 * つまりSecond Linker Memberが存在するケース(VCのlibなど).
		 */
		uint32_t mh_offset = 0;
		uint16_t symidx = 0;
		size_t idx;

		read_size = Znk_fread_blk( (uint8_t*)&num_of_members, sizeof(num_of_members), 1, fp );
		if( read_size != 1 ){
			/* error */
			Znk_printf_e( "Error : Cannot read second num_of_members\n" );
			goto FUNC_END;
		}
		read_member_size += sizeof(num_of_members);
		//Znk_printf_e( "num_of_members=[%I32u]\n", num_of_members );
		for( idx=0; idx<num_of_members; ++idx ){
			Znk_fread_blk( (uint8_t*)&mh_offset, sizeof(mh_offset), 1, fp );
			if( read_size != 1 ){
				/* error */
				Znk_printf_e( "Error : Cannot read second [%zu/%I32u]-th mh_offset\n", idx, num_of_members );
				goto FUNC_END;
			}
			read_member_size += sizeof(mh_offset);
			//Znk_printf_e( "mh_offset=[%08x]\n", mh_offset );
		}

		Znk_fread_blk( (uint8_t*)&num_of_symbols, sizeof(num_of_symbols), 1, fp );
		if( read_size != 1 ){
			/* error */
			Znk_printf_e( "Error : Cannot read second num_of_symbols\n" );
			goto FUNC_END;
		}
		read_member_size += sizeof(num_of_symbols);
		//Znk_printf_e( "num_of_symbols=[%I32u]\n", num_of_symbols );
		for( idx=0; idx<num_of_symbols; ++idx ){
			Znk_fread_blk( (uint8_t*)&symidx, sizeof(symidx), 1, fp );
			if( read_size != 1 ){
				/* error */
				Znk_printf_e( "Error : Cannot read second [%zu/%I32u]-th symidx\n", idx, num_of_symbols );
				goto FUNC_END;
			}
			read_member_size += sizeof(symidx);
			//Znk_printf_e( "symidx=[%u]\n", symidx );
		}

		if( !readSymnameList( num_of_symbols, symname, fp, read_member_size ) ){
			goto FUNC_END;
		}

		/***
		 * 次のMemberHeaderを読み込んでおく.
		 */
		readImageArchiveMemberHeader( &iamh, fp, debug );
	}

	/***
	 * IMAGE_ARCHIVE_LONGNAMES_MEMBERが存在するか否かを確認.
	 */
	if( Znk_memcmp( iamh.Name, (uint8_t*)IMAGE_ARCHIVE_LONGNAMES_MEMBER, 16 ) == 0 ){
		/* This is Long-name member */
		size_t size = getIAMHSize( &iamh );
		if( size % 2 ){ ++size; } /* for 0x0a padding */
		ZnkBfr_resize( long_name_body, size );
		//Znk_printf_e( "Long-name member size=[%zu]\n", size );
		Znk_fread( ZnkBfr_data(long_name_body), ZnkBfr_size(long_name_body), fp );

	} else {
		/***
		 * ヘッダの開始がIMAGE_ARCHIVE_LINKER_MEMBERとIMAGE_ARCHIVE_LONGNAMES_MEMBERのいずれでもなかった.
		 * これは既に通常のMemberHeaderを読み込んでいるいうこと.
		 */
		skip_hdr = true;
	}

	//sec = ZnkMyf_registSection( myf, "export_symbols", ZnkMyfSection_e_Lines );

	/***
	 * Scan objs member:
	 * COFFファイルのみ対応. OMFには対応していない.
	 * 従って、bccやdmcでのlibは今現在では分析できない.
	 */
	if( num_of_members ){
		/* VC lib */
		for( mem_idx=0; mem_idx<num_of_members; ++mem_idx ) {
			if( !readCoffObjMember( export_symbols, &iamh, fp, long_name_body, skip_hdr, debug ) ){
				break;
			}
			skip_hdr = false;
		}
	} else {
		/* UNIX ar format */
		while( !Znk_feof(fp) ){
			if( !readCoffObjMember( export_symbols, &iamh, fp, long_name_body, skip_hdr, debug ) ){
				break;
			}
			skip_hdr = false;
		}
	}
	//ZnkStrAry_sort( export_symbols );
	//ZnkStrAry_sort( ZnkMyfSection_lines(sec) );
	//ZnkMyf_save( myf, "test.myf" );

FUNC_END:
	Znk_fclose( fp );
	ZnkStr_delete( symname );
	ZnkStr_delete( obj_name );
	ZnkBfr_destroy( long_name_body );
	//ZnkMyf_destroy( myf );

	return result;
}
