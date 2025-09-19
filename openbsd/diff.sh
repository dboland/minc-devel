#!/bin/sh

. ../config.inc

TARGET=
SOURCE=${SRCDIR}
ARCHIVE=

diff_file()
{
	if ! diff -Nau "$SOURCE/$1" "$1" | sed "s:$SOURCE/::"; then
		echo "$1: no difference" >&2
	fi
}
diff_zip()
{
	unzip -p $ARCHIVE "$1" | diff -au - "$1"
}
diff_list()
{
	while read -r file; do
		if [[ $file == \#* ]]; then
			continue
		else
			diff_file "$file"
		fi
	done < $TARGET;
}
diff_dir()
{
	for file in $(find "$1" -type f); do
		if [[ $file == *.S ]]; then
			continue
		elif [[ $file == */Makefile ]]; then
			continue
		elif [[ $file == */BSDmakefile ]]; then
			continue
		elif [[ $file == */.gitignore ]]; then
			continue
		elif [ "$ARCHIVE" ]; then
			diff_zip "$file"
		else
			diff_file "$file"
		fi
	done
}

while [ -n "$1" ]; do
        case "$1" in
		-s|--source)
			SOURCE="$2"
			shift;
			;;
		-a|--archive)
			ARCHIVE="$2"
			shift
			;;
		*)
			TARGET="$1"
			;;
	esac
	shift
done

do_usage()
{
	echo "Usage: $0: [options] PATH"
	echo
	echo "Options"
	printf " -s,--source\t\tspecify source directory\n"
	printf " -a,--archive\t\tsource is ZIP archive\n"
}

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
