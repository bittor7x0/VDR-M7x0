all: dvbapi.ps dvbapi.pdf

TEXS=  dvbapi.tex devices.tex video.tex audio.tex ca.tex net.tex frontend.tex \
       demux.tex kdapi.tex examples.tex intro.tex title.tex dvbstb.ps

dvbapi.pdf: dvbapi.dvi
	dvipdf $<

dvbapi.ps: dvbapi.dvi
	dvips $<

dvbapi.dvi: dvbapi.bbl $(TEXS)
	-latex dvbapi
	-latex dvbapi

dvbapi.bbl: $(TEXS)
	-latex dvbapi
	-bibtex dvbapi
	-makeindex dvbapi

html: dvbapi.dvi
	latex2html -address "LinuxTV DVB API" -long_titles 4 -split 3 dvbapi.tex

%.ps: %.fig
	./fig2pstex $<

clean: 
	rm -f dvbapi.dvi 
	rm -f *.aux *.bbl *.blg *.idx *.ilg *.ind *.log *.out *.toc 
	rm -f *.pdf *.pst *.ps
	rm -rf dvbapi
