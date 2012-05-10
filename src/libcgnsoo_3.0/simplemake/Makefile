# Please adjust these two path.
# MLL_ROOT should point to a directory where the CGNS MLL library 
#          and headers version 2.4.5 or greater will be found
# INST_PREFIX should point to the installation directory for cgnsoo
#             subdirectories include/cgnsoo and lib will be created there
#          
MLL_ROOT=/home/magnan/technologie/cgns/cgnslib_2.4
INST_PREFIX=/local/magnan

INCLUDE=-I${MLL_ROOT}
CXXFLAGS=${INCLUDE}

OBJS  = file.o \
	node.o \
	structure_t.o \
	base.o \
	zone.o \
	zonegridconnectivity.o \
	element.o \
	gridconnectivity.o \
	gridconnectivityproperty.o \
	flowequationset.o \
	dataarray.o \
	gridcoordinates.o \
	zonebc.o \
	bc.o \
	bcdataset.o \
	bcproperty.o \
	family.o \
	familybc.o \
	geometryreference.o \
	flowsolution.o \
	userdefineddata.o \
	range.o \
	checkerror.o \
	quantity.o

SRCS  = ${OBJS:.o=.C}
DFILES= ${OBJS:.o=.d}
DEPS  = $(addprefix .deps/,$(DFILES) )

EXTRAS= COPYRIGHT LICENSE ChangeLog doc.cfg 

all: deps libcgnsoo.so

libcgnsoo.so: ${OBJS}
	${CXX} -shared -all $^ -o libcgnsoo.so

deps: ${DEPS}
	touch deps

.deps/%.d: %.C
	${CXX} -MM -MF $@ ${CXXFLAGS} $<

clean:
	rm -f *.o *.so *~ .deps/*.d deps

tarball:
	tar -cf - *.C *.h Makefile ${EXTRAS} | gzip >libcgnsoo.tgz

install:
	/home/math/config/install_math

doc: doc.cfg
	doxygen $?

-include ${DEPS}
