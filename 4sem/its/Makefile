README.pdf: README.md
	pandoc -V geometry:margin=2cm --variable urlcolor=blue $< -o $@

zip:
	rm -rf features/steps/__pycache__
	zip -r xbrabl04.zip requirements.txt features/
