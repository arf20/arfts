# Language spec v1.1

Lines that begin with '.' at the first column are arfts commands.
arfts commands are case insensitive. Some commands use '!' after '.' to denote
command end.

Reading notes:
 - \<thing>: denotes a mandatory thing
 - [thing]: denotes an optional thing
 - thing | other: denotes possible options

## Document Configuration

### .pagewidth \<n>

Default 80

### .pageheight \<n>

Default 63

### .tabstop \<n>

Default 4

### .indentparagraph on | off

Default on, indents 1 tab

### .header off | \<args>

Default off. Arguments can be from a single one to up to three. If single,
it may be preceded by "L" or "R" to left or right align it implicitly, default
is left. Width two and three, first is left aligned, last is right align, and
middle argument is center aligned if present.

args:
 - "\<str>": quoted string
 - pagenum: page number
 - pagenumext: page number preceded by "PAGE "

warns:
 - linebreak

### .footer off | \<args>

Same as .header

## Title page

### .title \<str>

### .author \<str>

### .date \<str>

### .titlepage

Generates title page with title, [author] and [date]

## Document structure

Structures of the same level end when the next begins. From top level to
bottom level:

### .part \<str>

Generates a part page with heading

### .chapter \<str>

Generates a chapter page with heading

### .section \<str>

Generates section heading

### .subsection \<str>

Generates subsection heading

### .subsubsection \<str>

Generates sub-sub-section heading


### .pagebreak

Inserts form feed


### .tableofcontents

Generates a table of contents page(s) from the document structure commands and
pages.

## Lists

Ends with paragraph

### .itemize

Begins unordered list

### .enumerate

Begins ordered list

### .item \<str>

List item 

## Tables

### .table [table description]

Begins table

### .tr, .!tr

Row

### .th \<str>

Header

### .td \<str>

Cell

## Graphics or whatever

### .fig [figure text], .!fig

Preformat block for whatever

## Inline commands

Also starts by '.' but may be inside a paragraph.

### .br

Inserts line break

### .tab

Inserts indentation, useful before .table or .fig for example

### .ref \<refname>

Inserts superscript reference to bibliography

### .footnote \<footnotename>

Inserts superscript reference to foot note

### .footnotedef \<footnotename> \<text>

Defines foot note to be inserted at the bottom of the page where it is
.footnote'ed after body before footer

## Bibliography

### .bibliography

Inserts bibliography section page(s)

### .refdef \<refname> \<idk ill figure it out later>

Defines bibliography entry to be inserted at .bibliography

