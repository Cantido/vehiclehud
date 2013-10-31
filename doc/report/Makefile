sections = executive-summary.tex project-team.tex introduction.tex \
project-objectives.tex project-specifications.tex detailed-design.tex \
experimental-results.tex timeline-and-deliverables.tex budget.tex \
references.tex

all: report.pdf

report.pdf: report.tex $(sections) report.toc
	pdflatex report.tex

report.toc: report.tex $(sections)
	pdflatex report.tex

clean:
	rm -rf *.aux *.toc *.log *.pdf *.synctex.gz