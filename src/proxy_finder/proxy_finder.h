#ifndef INCLUDE_GUARD__proxy_finder_h__
#define INCLUDE_GUARD__proxy_finder_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

bool
ProxyFinder_download( ZnkStr result_filename,
		const char* hostname, const char* url_underpath, const char* target, ZnkStr ermsg );
bool
ProxyFinder_analysis( const char* analysing_filename, const char* parent_proxy_filename,
		const char* ip_ptn_begin,   const char* ip_ptn_end,
		const char* port_ptn_begin, const char* port_ptn_end,
		bool is_hex_port, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
