#!/bin/sh

. ../config.inc

VERBOSE=
TARGET=
SOURCE=${XPREFIX}

diff_file()
{
	if [ "$VERBOSE" ]; then
		echo "$1"
	fi
	diff -Nau "$SOURCE/$1" "$1" | sed "s:$SOURCE/::"
}
diff_list()
{
	while read -r file; do
		if [[ $file == \#* ]]; then
			continue
		elif ! diff_file "$file"; then
			exit 2
		fi
	done < $TARGET;
}
diff_dir()
{
	for file in $(find "$1" -type f); do
		if [[ $file == *.o ]]; then
			continue
		elif [[ $file == *.a ]]; then
			continue
		elif [[ $file == *.S ]]; then
			continue
		elif [[ $file == */Makefile ]]; then
			continue
		elif [[ $file == */BSDmakefile ]]; then
			continue
		elif [[ $file == */.gitignore ]]; then
			continue
		elif ! diff_file "$file"; then
			exit 2
		fi
	done
}
do_usage()
{
	echo "Usage: $0: [options] PATH"
	echo
	echo "Options"
	echo " -s,--source\t\t\tspecify source directory"
	echo " -v,--verbose\t\t\tbe verbose"
}

while [ -n "$1" ]; do
        case "$1" in
		-s|--source)
			SOURCE="$2"
			shift;
			;;
		-v|--verbose)
			VERBOSE=yes
			;;
		*)
			TARGET="$1"
			;;
	esac
	shift
done

if [ -z "$TARGET" ]; then
	do_usage
elif ! [ -d "$SOURCE" ]; then
	echo "$SOURCE: No such directory"
elif [ -d "$TARGET" ]; then
	diff_dir "$TARGET"
elif [[ $TARGET == *.master ]]; then
	diff_list
elif [ -f "$TARGET" ]; then
	diff_file "$TARGET"
fi
