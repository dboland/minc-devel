#!/bin/sh

DEVROOT=$(dirname $0)

. ${DEVROOT}/config.inc

TARGET=
SOURCE=${SRCDIR}
SRCTYPE=dir
LIST=
DEPTH=
COMMAND=diff

diff_head()
{
	echo "--- $1/.patch	1970-01-01 01:00:00 +0100"
	echo "+++ $1/.patch	2025-09-12 11:34:13 +0200"
	echo "@@ -0,0 +1 @@"
	echo "+This directory has been patched."
}
diff_file()
{
	diff -Nau "$SOURCE/$1" "$1" | sed "s:$SOURCE/::"
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
	diff_head "$1"
	for file in $(find "$1" -type f); do
	case $file in
		*.o|*.exe|*.a)
			echo "$file: cannot diff: directory not clean" >&2
			exit 1
			;;
		*.S|*/Makefile|*/BSDmakefile|*/.gitignore)
			continue
			;;
		*.minc.*)
			#echo "$file: MinC replacement" >&2
			;;
		*)
			diff -au "$SOURCE/$file" "$file" | sed "s:$SOURCE/::"
			;;
	esac
	done
}
diff_zip()
{
	diff_head "$1"
	for file in $(find "$1" -type f -newer ${SOURCE}); do
		if [[ $file == *.manlint ]]; then
			echo "$file: cannot diff: directory not clean" >&2
			exit 1
		elif [[ $file == *.out ]]; then
			continue
		elif [[ $file == *.save ]]; then
			continue
		elif [[ $file == */.patch ]]; then
			continue
		elif [[ $file == *.a ]]; then
			continue
		elif [ "$LIST" ]; then
			echo "$file"
		else
			unzip -p $SOURCE "$file" | diff -au - "$file"
		fi
	done
}
diff_tar()
{
	echo "tar: cannot diff: finding modified files..." >&2
	for file in $(find "$1" -type f -newer ${SOURCE} ${DEPTH}); do
		if [[ $file == *.o ]]; then
			continue
		else
			echo "$file"
		fi
	done
}

set_type()
{
	if [[ $1 == *.zip ]]; then
		SRCTYPE=zip
	elif [[ $1 == *.tar.gz ]]; then
		SRCTYPE=tar
	else
		echo "$1: unsupported compression"
		exit 1
	fi
	SOURCE="$1"
}

while [ -n "$1" ]; do
        case "$1" in
		-s|--source)
			SOURCE="$2"
			shift;
			;;
		-a|--archive)
			set_type "$2"
			shift
			;;
		-l|--list)
			LIST=yes
			;;
		-d|--depth)
			DEPTH="-maxdepth $2"
			shift
			;;
		list|usage)
			COMMAND=$1
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
	printf " -s,--source\t\tspecify source directory [${SOURCE}]\n"
	printf " -a,--archive\t\tspecify ZIP archive as source\n"
	printf " -l,--list\t\tprint files only, do not diff\n"
	printf " -d,--depth\t\tlimit file search to depth\n"
}
do_list()
{
	find . -name .patch
}
do_diff()
{
	if [ -z "$TARGET" ]; then
		do_usage
	elif [[ $TARGET == *.master ]]; then
		diff_list
	elif [ -d "$TARGET" ]; then
		diff_$SRCTYPE "$TARGET"
	else
		echo "$TARGET: not a directory"
	fi
}

do_${COMMAND}
