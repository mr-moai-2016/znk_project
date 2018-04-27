" Vim syntax file
" Language:	myf file
" Maintainer:   Zenkaku (@)
" URL:		https://github.com/mr-moai-2016/znk_project
" Last Change:	2016/03/25

if version < 600
	syntax clear
elseif exists("b:current_syntax")
	finish
endif

syn region  myfLinesBegin matchgroup=myfSectionStart start="^@@L" end="$" keepend
syn region  myfVarsBegin  matchgroup=myfSectionStart start="^@@V" end="$" keepend
syn region  myfPrimsBegin matchgroup=myfSectionStart start="^@@P" end="$" keepend
syn match   myfSectionEnd "^@@\."
syn region  myfDefQuote   matchgroup=myfSectionStart start="^@def_quote" end="$" keepend
syn region  myfCommentL	  start="^#" skip="\\$" end="$" keepend

syn region  myfString  start=+L\='+ skip=+\\\\\|\\"\|\\$+ excludenl end=+'+ end='$'
syn region  myfString  start=+L\="+ skip=+\\\\\|\\"\|\\$+ excludenl end=+"+ end='$'
syn region  myfString  matchgroup=myfStringBegin start=+L\=\['+ skip=+\\\\\|\\"\|\\$+ excludenl end=+'\]+ end='$'

" Default highlighting
if version >= 508 || !exists("did_myf_syntax_inits")
  if version < 508
    let did_myf_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif
  HiLink myfLinesBegin    Include
  HiLink myfVarsBegin     Include
  HiLink myfPrimsBegin    Include
  HiLink myfSectionEnd    Statement
  HiLink myfDefQuote      Include
  HiLink myfSectionStart  Statement
  HiLink myfString        String
  HiLink myfStringBegin   Operator
  HiLink myfCommentL      Comment
  delcommand HiLink
endif

let b:current_syntax = "myf"

" vim: ts=8
