#!/bin/sh
rm -f $2
for dir in $1/*; do
	printf "%-60s\t%+8s\t%+5s\n" \""$dir"\" $(du -sh "$dir" | awk '{print $1}') $(find "$dir" -type f | wc -l) >> "$2"
done
