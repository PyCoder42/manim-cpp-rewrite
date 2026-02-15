REPO_ROOT := $(abspath ..)
DOCS_TOOL := $(REPO_ROOT)/tools/docs/build_docs.sh
I18N_TOOL := $(REPO_ROOT)/tools/docs/check_i18n_catalogs.sh

.PHONY: help check-i18n check build html clean

help:
	@echo "Documentation targets:"
	@echo "  make check      - verify docs configs exist (non-building check)"
	@echo "  make check-i18n - verify locale catalogs/templates are present"
	@echo "  make build      - build mdBook and Doxygen docs"
	@echo "  make html       - alias for make build"
	@echo "  make clean      - remove generated docs output"

check-i18n:
	@"$(I18N_TOOL)" "$(REPO_ROOT)"

check:
	@"$(DOCS_TOOL)" --check-only "$(REPO_ROOT)"

build html:
	@"$(DOCS_TOOL)" "$(REPO_ROOT)"

clean:
	@rm -rf "$(REPO_ROOT)/docs/book/book"
	@rm -rf "$(REPO_ROOT)/docs/api/build"
