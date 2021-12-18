TgtAdoc  := index
TgtPdf := MoonGL-RefMan

Stylesdir  = ./
Stylesheet = colony.css
Style = -a stylesheet=$(Stylesheet) -a stylesdir=$(Stylesdir)
Style = 

ifdef TgtAdoc
# one file only
Src = $(TgtAdoc).adoc
else
# all .adoc files
Src  := $(wildcard *.adoc)
endif

Xml  := $(Src:.adoc=.xml)
Html := $(Src:.adoc=.html)
Pdf  := $(Src:.adoc=.pdf)

default: html

check:

clean:
	@-rm -f *~
	@-rm -f *.pdf
	@-rm -f *.pdfmarks
	@-rm -f *.xml
	@-rm -f *.html
	@-rm -f *.epub
	@-rm -f *.fo
	@-rm -f *.log

install:

html: clean
	@asciidoctor $(Style)  $(Src)

preview: html
	@google-chrome $(Html)

ifdef TgtAdoc
# one file only
pdf: clean
	@asciidoctor-pdf $(TgtAdoc).adoc -o $(TgtPdf).pdf
else
# all .adoc files
pdf: clean
	@asciidoctor-pdf -a pdf-style=asciidoctor $(TgtAdoc).adoc
endif

docs: html pdf
