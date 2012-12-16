SHELL = /bin/sh
JOB ?= parcomp
TEX_MAIN ?= index
# trailing / is important below
OUTDIR := output/
#shell escape needed for epstopdf, svg2pdf etc.
TEX := pdflatex -shell-escape -output-directory $(OUTDIR) -jobname $(JOB)
BIB := bibtex
IDX := makeindex
ACR := makeglossaries
VIEW := "gnome-open"
CWD := $(shell pwd)
TEX_OUT := $(OUTDIR)$(JOB).pdf


#all: prereq $(TEX_OUT)
all: view

fast: prereq
	$(TEX) $(TEX_MAIN).tex
	"$(VIEW)" "$(CWD)/$(TEX_OUT)"

view: prereq $(TEX_OUT)
	"$(VIEW)" "$(CWD)/$(TEX_OUT)"

clean:
	rm -rf $(OUTDIR)*.aux $(OUTDIR)*.bbl $(OUTDIR)*.idx $(OUTDIR)*.ilg $(OUTDIR)*.lot $(OUTDIR)*.blg $(OUTDIR)*.alg $(OUTDIR)*.ind $(OUTDIR)*.toc $(OUTDIR)*.acl $(OUTDIR)*.acn $(OUTDIR)*.acr $(OUTDIR)*.out $(OUTDIR)*.log $(OUTDIR)*.gls $(OUTDIR)*.glo $(OUTDIR)*.glg $(OUTDIR)*.ist $(OUTDIR)*.brf $(OUTDIR)images/*pdf $(TEX_OUT)

$(TEX_OUT): *.tex chapters/*tex *.bib
	$(TEX) $(TEX_MAIN)
	$(BIB) $(OUTDIR)$(JOB)
	#$(IDX) $(OUTDIR)$(JOB).idx
	#$(ACR) $(OUTDIR)$(JOB)
	$(TEX) $(TEX_MAIN)
	#$(ACR) $(OUTDIR)$(JOB)
	$(TEX) $(TEX_MAIN)

prereq:
	@mkdir $(OUTDIR) 2>/dev/null || true

.PHONY = clean view all fast prereq
