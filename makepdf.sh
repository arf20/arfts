#!/bin/sh
iconv -f utf8 -t latin1 $1 | enscript -l -M A4 -f Courier@11 -p - | ps2pdf - - > $1.pdf

