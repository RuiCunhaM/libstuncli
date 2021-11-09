SUBDIRS = include src #examples

ACLOCAL_AMFLAGS = -I m4

## Doxygen stuff
@DX_RULES@
MOSTLYCLEANFILES = $(DX_CLEANFILES)

EXTRA_DIST = README.md Doxyfile LICENSES

README: $(top_srcdir)/README.md
	@test -z "$(PANDOC)" || $(PANDOC) --from gfm --to plain --output=$@ $<

dist-hook: README

MAINTAINERCLEANFILES = README
