## Makefile.am -- Process this file with automake to produce Makefile.in
## Fortement inspire d'un exemple du Autobook
MAINTAINERCLEANFILES	= Makefile.in common.h stamp-common

# CGNSLIBHOME is defined through configure 
CGNSLIB_HOME     =  $(CGNSLIBHOME)
CGNSLIB_INCLUDE  =  $(CGNSLIB_HOME)/include

INCLUDES		= -I$(top_builddir) -I$(top_srcdir)                \
				  -I$(CGNSLIB_INCLUDE)


AM_CXXFLAGS    = $(CXXFLAGS) 


pkginclude_HEADERS 		=   file.H                   \
				    cgnsmll.H                \
				    node.H                   \
				    structure_t.H            \
				    quantity.H               \
				    Array.H                  \
				    cgnsoo.H                 \
				    checkerror.H             \
				    dimensionalexponents.H   \
				    enum_table.H             \
				    range.H 



## @start 1
lib_LTLIBRARIES         = 	libcgnsoo3.la
libcgnsoo3_la_LIBADD	    = 	#### $(top_builddir)/replace/libreplace.la

libcgnsoo3_la_SOURCES    = 	\
				base.C        \
				bc.C          \
				bcdataset.C   \
				bcproperty.C  \
				checkerror.C  \
				dataarray.C   \
				element.C     \
				family.C      \
				familybc.C    \
				file.C        \
				node.C        \
				quantity.C    \
				range.C       \
				structure_t.C \
				zone.C        \
				zonebc.C      \
				userdefineddata.C   \
				flowequationset.C   \
				flowsolution.C      \
				geometryreference.C \
				gridconnectivity.C  \
				gridcoordinates.C   \
				gridconnectivityproperty.C \
				zonegridconnectivity.C


## @end 1

# Don't distribute common.h, since it is build host dependent!
dist-hook:
	
#
# Clean local: Surtout des fichiers de Emacs
clean-local:
	-rm -rf *~ \#*\# *.ii *.ti Template.dir

