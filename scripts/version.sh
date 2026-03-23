#!/bin/sh

<<<<<<< HEAD
[ -f neverball-version.txt ] || {
    echo 'neverball-version.txt is missing, wrong directory?' 1>&2
    exit 1
}

# neverball-1.6.0-1234-gdeadbeef -> 1.6.0 (deadbeef)
version="$(git describe --tags --first-parent 2>/dev/null | sed -e 's/^neverball-//' -e 's/-[0-9]\+-g\(.*\)$/ (\1)/')"

if [ -z "$version" ]; then
    version="$(cat neverball-version.txt 2> /dev/null)"

    if [ -z "$version" ]; then
        echo 'neverball-version.txt is empty, WTF?' 1>&2
        exit 1
    fi
fi

# Safeguard against a syntax error.
version="$(echo "$version" | sed 's/"//g')"

header=share/version.h
cached=share/version.d

if [ "$version" != "$(cat "$cached" 2> /dev/null)" ]; then
    cat > "$header" <<EOF
#ifndef VERSION_H
#define VERSION_H 1
#define VERSION "$version"
#endif
EOF
    echo "$version" > "$cached" 2> /dev/null
fi

echo "$version"
=======
BUILD="$1"
VERSION="$2"
TEMPLATE_FILE="$3"
HEADER_FILE="$4"
CACHE_FILE="$5"

LC_ALL=C
export LC_ALL

svn_version()
{
    svn_rev="$(svnversion . /svn/neverball/trunk | tr : +)"
    [ "$svn_rev" != "exported" ] && echo "r$svn_rev"
}

if [ "$BUILD" != "release" ]; then
    VERSION="$(svn_version || date -u +"%Y-%m-%d" || echo "$VERSION-dev")"
fi

if [ "$VERSION" != "$(cat "$CACHE_FILE" 2> /dev/null)" ]; then
    sed 's,0\.0\.0,'"$VERSION"',' < "$TEMPLATE_FILE" > "$HEADER_FILE"
    echo "$VERSION" > "$CACHE_FILE" 2> /dev/null
fi

echo "$VERSION"
>>>>>>> origin/csy-extras
