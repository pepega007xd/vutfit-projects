packdirname = xbrabl04_xhumld00_xjasek18_xkrejz07
packdir = ../../$(packdirname)

.PHONY: all pack clean test doc run profile

all:
	cargo tauri build --bundles deb && cargo deb --package stddev

pack: all doc
	# cleanup
	rm -rf $(packdir)
	mkdir -p $(packdir)/doc $(packdir)/install $(packdir)/repo
	# installers
	cp target/release/bundle/deb/*.deb $(packdir)/install
	cp target/debian/*.deb $(packdir)/install
	# documenation
	cp -r target/doc/* $(packdir)/doc
	# removing build files before copying repo
	rm -rf target frontend/dist
	cp -r .. $(packdir)/repo
	zip -r $(packdirname).zip $(packdir)
	rm -rf $(packdir)
	
clean:
	cargo clean
	rm -rf frontend/dist

test:
	cargo test --package mathlib

doc:
	cargo doc --workspace --no-deps

run:
	cargo tauri dev

profile:
	cargo deb --package stddev
