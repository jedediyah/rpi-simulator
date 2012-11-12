#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ql0001.o \
	${OBJECTDIR}/Body_trimesh.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/Simulation.o \
	${OBJECTDIR}/Body_Face.o \
	${OBJECTDIR}/Body_Edge.o \
	${OBJECTDIR}/Body_Vertex.o \
	${OBJECTDIR}/Contact.o \
	${OBJECTDIR}/Body_object.o \
	${OBJECTDIR}/SimulationEnvironment.o \
	${OBJECTDIR}/Body_polymesh.o \
	${OBJECTDIR}/CollisionDetection.o \
	${OBJECTDIR}/Body_sphere.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-IPATH
CXXFLAGS=-IPATH

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-I/usr/include -L/usr/lib -lglut -lGL -lGLU -lX11 -larmadillo

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/rpi-sim

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/rpi-sim: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/rpi-sim ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/ql0001.o: ql0001.f 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.f) -g -o ${OBJECTDIR}/ql0001.o ql0001.f

${OBJECTDIR}/Body_trimesh.o: Body_trimesh.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Body_trimesh.o Body_trimesh.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/Simulation.o: Simulation.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Simulation.o Simulation.cpp

${OBJECTDIR}/Body_Face.o: Body_Face.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Body_Face.o Body_Face.cpp

${OBJECTDIR}/Body_Edge.o: Body_Edge.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Body_Edge.o Body_Edge.cpp

${OBJECTDIR}/Body_Vertex.o: Body_Vertex.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Body_Vertex.o Body_Vertex.cpp

${OBJECTDIR}/Contact.o: Contact.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Contact.o Contact.cpp

${OBJECTDIR}/Body_object.o: Body_object.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Body_object.o Body_object.cpp

${OBJECTDIR}/SimulationEnvironment.o: SimulationEnvironment.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/SimulationEnvironment.o SimulationEnvironment.cpp

${OBJECTDIR}/Body_polymesh.o: Body_polymesh.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Body_polymesh.o Body_polymesh.cpp

${OBJECTDIR}/CollisionDetection.o: CollisionDetection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/CollisionDetection.o CollisionDetection.cpp

${OBJECTDIR}/Body_sphere.o: Body_sphere.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Body_sphere.o Body_sphere.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/rpi-sim
	${RM} *.mod

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
