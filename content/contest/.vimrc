set smartindent
syntax on
set tabstop=2
set shiftwidth=2
set number
set smartcase
set incsearch
set hlsearch

" optional
set mouse=a
set clipboard=unnamed #plus

" Select region and then type :Hash to hash your selection.
" Useful for verifying that there aren't mistypes.
ca Hash w !cpp -dD -P -fpreprocessed \| tr -d '[:space:]' \
 \| sh -c 'if command -v md5sum >/dev/null; then md5sum; else md5 -q; fi' \
 \| cut -c-6
