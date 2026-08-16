# Hashes a file, ignoring all whitespace and comments. Use for
# verifying that code was correctly typed.
cpp -dD -P -fpreprocessed | tr -d '[:space:]' | {
	if command -v md5sum >/dev/null 2>&1; then md5sum
	else md5 -q
	fi
} | cut -c-6
