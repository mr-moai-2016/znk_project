#include <Znk_str_ary.h>
#include <Znk_missing_libc.h>
#include <Znk_myf.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if 0
static void
setScreenBufferSize( HANDLE hStdOutput, const size_t x_char_num, const size_t y_char_num )
{
	SMALL_RECT rc = { 0, 0, 0, 0 };
	COORD      coord;
	rc.Right  = x_char_num;
	rc.Bottom = y_char_num;
	coord.X = rc.Right  + 1;
	coord.Y = rc.Bottom + 1;
	SetConsoleScreenBufferSize( hStdOutput, coord );
	SetConsoleWindowInfo( hStdOutput, TRUE, &rc );
}
#endif

static void
redrawMenu( ZnkStrAry menu_list, size_t selected_idx, HANDLE hStdOutput, CONSOLE_SCREEN_BUFFER_INFO* screenBuffer, COORD* coordCursor )
{
	size_t i;
	size_t size = ZnkStrAry_size(menu_list);
	ZnkStr str;
	DWORD  dwWriteByte;
	//COORD  coordCursorTmp = *coordCursor;

#if 0
	SetConsoleCursorPosition(hStdOutput, coordCursorTmp);
#endif

	for( i=0; i<size; ++i ){
		str = ZnkStrAry_at( menu_list, i );
		if( i==selected_idx ){
			SetConsoleTextAttribute(hStdOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		}
		WriteConsole(hStdOutput, ZnkStr_cstr(str)+3, ZnkStr_leng(str)-3, &dwWriteByte, NULL);
		WriteConsole(hStdOutput, "\n", 1, &dwWriteByte, NULL);
		if( i==selected_idx ){
			SetConsoleTextAttribute(hStdOutput, screenBuffer->wAttributes);
		}
	}	

	/***
	 * coordCursorTmp.Y:
	 * この値はコマンドプロンプトで設定された画面バッファサイズの高さが最大値であることがわかる.
	 * よって、単に SetConsoleCursorPosition(hStdOutput, *coordCursor); で元の値に復元すると
	 * 既にWriteConsoleが何回か呼び出されておりその間にバッファサイズの高さの最大値を超えるなどした場合、
	 * 問題が生じるケースがある.
	 * そこでここであらためてカレントのカーソル位置を取得し、その位置はmenu_listの一番下にあることから逆算して
	 * ただしいカーソル位置を導きだす方法をとる.
	 */
	{
		COORD coordCursorBegin;
		GetConsoleScreenBufferInfo(hStdOutput, screenBuffer);
		coordCursorBegin = screenBuffer->dwCursorPosition;
		coordCursorBegin.Y -= size; /* とりあえず現段階ではメニューの最初の位置固定とする */
		SetConsoleCursorPosition( hStdOutput, coordCursorBegin );
	}
#if 0
	SetConsoleCursorPosition(hStdOutput, *coordCursor);
#endif
}

static bool
execLines( const ZnkStrAry lines, size_t begin_idx )
{
	const size_t lines_size = ZnkStrAry_size(lines);
	size_t line_idx;
	const char* cmd;
	for( line_idx=begin_idx; line_idx<lines_size; ++line_idx ){
		cmd = ZnkStrAry_at_cstr(lines, line_idx);
		if( ZnkS_eq( cmd, "quit" ) ){
			return false;
		}
		system( cmd );
	}
	return true;
}

static void 
showOpening( HANDLE hStdOutput, CONSOLE_SCREEN_BUFFER_INFO* screenBuffer, COORD *coordCursor, ZnkStrAry opening )
{
	execLines( opening, 0 );
	GetConsoleScreenBufferInfo(hStdOutput, screenBuffer);
	*coordCursor = screenBuffer->dwCursorPosition;
}
bool
test_curpos( int* ans_selected_idx )
{
	char         szBuf[256];
	HANDLE       hStdOutput, hStdInput;
	COORD        coordCursor;
	COORD        coordCursorBegin;
	DWORD        dwWriteByte, dwReadEvent;
	INPUT_RECORD inputRecord;
	ZnkStrAry menu_list = ZnkStrAry_create( true );
	CONSOLE_SCREEN_BUFFER_INFO screenBuffer = { { 0 } };
	char         myf_filename[ 256 ] = "auto_trigger.myf";
	ZnkMyf myf;
	ZnkStrAry opening = NULL;
	int selected_idx = *ans_selected_idx;
	int offset_y = 0;
	
	{
		char execfile_path[ 256 ];
		if( GetModuleFileName( NULL, execfile_path, sizeof(execfile_path) ) ){
			char* p = strrchr( execfile_path, '\\' );
			if( p ){
				*p = '\0';
				Znk_snprintf( myf_filename, sizeof(myf_filename), "%s\\auto_trigger.myf", execfile_path );
			}
		}
	}
	
	myf = ZnkMyf_create();
	if( !ZnkMyf_load( myf, myf_filename ) ){
		Znk_printf_e( "Config myf file=[%s] is not installed.\n", myf_filename ); 
		return false;
	}
	AllocConsole();
	
	hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdInput = GetStdHandle(STD_INPUT_HANDLE);

	{
		opening = ZnkMyf_find_lines( myf, "opening" );
	}

	{
		size_t num_of_section = ZnkMyf_numOfSection( myf );
		size_t sec_idx;
		ZnkMyfSection sec;
		ZnkStrAry lines;
		ZnkStr line;
		for( sec_idx=0; sec_idx<num_of_section; ++sec_idx ){
			sec = ZnkMyf_atSection( myf, sec_idx );
			lines = ZnkMyfSection_lines( sec );
			if( ZnkStrAry_size(lines) > 0 ){
				line = ZnkStrAry_at(lines, 0);
				if( ZnkStr_isBegin(line, "::") ){
					ZnkStrAry_push_bk_cstr( menu_list, ZnkStr_cstr(line), ZnkStr_leng(line) );
				}
			}
		}
	}

	{
		GetConsoleScreenBufferInfo(hStdOutput, &screenBuffer);
		coordCursorBegin = screenBuffer.dwCursorPosition;
	}
	showOpening( hStdOutput, &screenBuffer, &coordCursor, opening );

	redrawMenu( menu_list, selected_idx, hStdOutput, &screenBuffer, &coordCursor );

	offset_y = ZnkStrAry_size(menu_list);
	
	for (;;) {
		if (!ReadConsoleInput(hStdInput, &inputRecord, 1, &dwReadEvent))
			continue;

		if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
			int vk_code = inputRecord.Event.KeyEvent.wVirtualKeyCode;
			switch( vk_code ){
			case '0' : case '1' : case '2' : case '3' : case '4' :
			case '5' : case '6' : case '7' : case '8' : case '9' :
				selected_idx = vk_code-'0';
				/* fall-thr */
			case VK_RETURN:
			{
				ZnkStr menu_item = NULL;
				bool switch_button = false;
				if( selected_idx < (int)ZnkStrAry_size(menu_list) ){
					menu_item = ZnkStrAry_at(menu_list, selected_idx);
				}
				if( menu_item && ZnkStr_isBegin( menu_item, "::S " ) ){
					switch_button = true;
					coordCursor.Y = coordCursorBegin.Y;
				} else {
					coordCursor.Y = coordCursor.Y + (SHORT)offset_y;
				}
				SetConsoleCursorPosition(hStdOutput, coordCursor);
				{
					ZnkMyfSection sec = ZnkMyf_atSection( myf, selected_idx+1 );
					ZnkStrAry lines = ZnkMyfSection_lines( sec );
					if( !execLines( lines, 1 ) ){
						goto LOOP_END;
					}
				}
				if( switch_button ){
				} else {
					WriteConsole(hStdOutput, "\ndone.\n", 7, &dwWriteByte, NULL);
					//Znk_printf( "\ndone.\n" );
				}

				{
					GetConsoleScreenBufferInfo(hStdOutput, &screenBuffer);
					coordCursorBegin = screenBuffer.dwCursorPosition;
				}
				showOpening( hStdOutput, &screenBuffer, &coordCursor, opening );
				redrawMenu( menu_list, selected_idx, hStdOutput, &screenBuffer, &coordCursor );
				offset_y = ZnkStrAry_size(menu_list);
				break;
			}
			case VK_ESCAPE:
			case 'Q':
			{
				coordCursor.Y = coordCursor.Y + (SHORT)offset_y;
				SetConsoleCursorPosition(hStdOutput, coordCursor);
				goto LOOP_END;
			}
			case VK_LEFT:
			case 'H':
			{
				/* none */
				break;
			}
			case VK_RIGHT:
			case 'L':
			{
				/* none */
				break;
			}
			case VK_UP:
			case 'K':
			{
				if( --selected_idx < 0 ){
					selected_idx = 0;
				} else {
					redrawMenu( menu_list, selected_idx, hStdOutput, &screenBuffer, &coordCursor );
				}
				offset_y = ZnkStrAry_size(menu_list);
				break;
			}
			case VK_DOWN:
			case 'J':
			{
				int menu_size = ZnkStrAry_size(menu_list);
				if( ++selected_idx >= menu_size ){
					selected_idx = menu_size-1;
				} else {
					redrawMenu( menu_list, selected_idx, hStdOutput, &screenBuffer, &coordCursor );
				}
				offset_y = ZnkStrAry_size(menu_list);
				break;
			}
			default:
			{
				//if (IsCharAlpha(inputRecord.Event.KeyEvent.wVirtualKeyCode)) {
				if( isalpha( inputRecord.Event.KeyEvent.wVirtualKeyCode) ) {
					sprintf(szBuf, "%c", inputRecord.Event.KeyEvent.wVirtualKeyCode);
					WriteConsoleOutputCharacter(hStdOutput, szBuf, strlen(szBuf), coordCursor, &dwWriteByte);
				}
			}
			}
		}
	}
LOOP_END:

	FreeConsole();
	ZnkMyf_destroy( myf );

	*ans_selected_idx = selected_idx;
	return true;
}

int main( void )
{
	int selected_idx = 1;
	if( !test_curpos( &selected_idx ) ){
		return -1;
	}
	return selected_idx;
}
