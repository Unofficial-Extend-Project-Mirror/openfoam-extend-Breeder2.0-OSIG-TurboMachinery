## Makefile.am -- Process this file with automake to produce Makefile.in
## Fortement inspire d'un exemple du autobook

## @start 1
AUX_DIST                = $(ac_aux_dir)/config.guess \
			$(ac_aux_dir)/config.sub \
			$(ac_aux_dir)/install-sh \
			$(ac_aux_dir)/ltconfig \
			$(ac_aux_dir)/ltmain.sh \
			$(ac_aux_dir)/mdate-sh \
			$(ac_aux_dir)/missing \
			$(ac_aux_dir)/mkinstalldirs
AUX_DIST_EXTRA          = $(ac_aux_dir)/readline.m4 \
			$(ac_aux_dir)/sys_errlist.m4 \
                        $(ac_aux_dir)/sys_siglist.m4
EXTRA_DIST		= bootstrap

## @end 1
AUTOMAKE_OPTIONS 	= gnits
SUBDIRS 		= src 
## @start 1
MAINTAINERCLEANFILES 	= Makefile.in aclocal.m4 configure config-h.in \
			stamp-h.in $(AUX_DIST)

## @end 1
ACLOCAL			= aclocal -I $(ac_aux_dir)

## @start 2
docdir			= $(top_builddir)/doc

html:
	@echo Making $@ in $(docdir)
	@cd $(docdir) && make $@
## @end 2
## @start 1
dist-hook:
	(cd $(distdir) && mkdir $(ac_aux_dir))
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
	  cp $$file $(distdir)/$$file; \
	done
## @end 1
clean-local:
	-rm -rf *~
