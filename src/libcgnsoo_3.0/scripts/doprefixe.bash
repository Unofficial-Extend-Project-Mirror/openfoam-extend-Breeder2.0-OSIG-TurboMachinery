#!/bin/bash

HEADER=COPYRIGHT

function prefixe()
{
	f=$1
	cp $f $f.orig
	cat $HEADER $f.orig >$f
}

if [ $# -eq 0 ]; then
	echo 'usage: $0 files';
	exit 0;
fi

for f in $*; do
	prefixe $f;
done