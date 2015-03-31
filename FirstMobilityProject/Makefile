#
# OMNeT++/OMNEST Makefile for FirstMobilityProject
#
# This file was generated with the command:
#  opp_makemake -f --deep -O out -IC:/omnetpp-4.6/samples/inet/src/linklayer/ieee80211/radio -IC:/omnetpp-4.6/samples/inet/src/networklayer/routing/aodv -IC:/omnetpp-4.6/samples/inet/src/networklayer/common -IC:/omnetpp-4.6/samples/inet/src -IC:/omnetpp-4.6/samples/inet/src/networklayer/icmpv6 -IC:/omnetpp-4.6/samples/inet/src/world/obstacles -IC:/omnetpp-4.6/samples/inet/src/networklayer/contract -IC:/omnetpp-4.6/samples/inet/src/networklayer/xmipv6 -IC:/omnetpp-4.6/samples/inet/src/networklayer/autorouting/ipv4 -IC:/omnetpp-4.6/samples/inet/src/util -IC:/omnetpp-4.6/samples/inet/src/transport/contract -IC:/omnetpp-4.6/samples/inet/src/linklayer/common -IC:/omnetpp-4.6/samples/inet/src/status -IC:/omnetpp-4.6/samples/inet/src/linklayer/radio/propagation -IC:/omnetpp-4.6/samples/inet/src/linklayer/ieee80211/radio/errormodel -IC:/omnetpp-4.6/samples/inet/src/linklayer/radio -IC:/omnetpp-4.6/samples/inet/src/util/headerserializers/tcp -IC:/omnetpp-4.6/samples/inet/src/networklayer/ipv4 -IC:/omnetpp-4.6/samples/inet/src/mobility/contract -IC:/omnetpp-4.6/samples/inet/src/util/headerserializers/ipv4 -IC:/omnetpp-4.6/samples/inet/src/base -IC:/omnetpp-4.6/samples/inet/src/util/headerserializers -IC:/omnetpp-4.6/samples/inet/src/world/radio -IC:/omnetpp-4.6/samples/inet/src/linklayer/ieee80211/mac -IC:/omnetpp-4.6/samples/inet/src/networklayer/ipv6 -IC:/omnetpp-4.6/samples/inet/src/transport/sctp -IC:/omnetpp-4.6/samples/inet/src/util/headerserializers/udp -IC:/omnetpp-4.6/samples/inet/src/networklayer/ipv6tunneling -IC:/omnetpp-4.6/samples/inet/src/util/headerserializers/ipv6 -IC:/omnetpp-4.6/samples/inet/src/battery/models -IC:/omnetpp-4.6/samples/inet/src/applications/pingapp -IC:/omnetpp-4.6/samples/inet/src/linklayer/contract -IC:/omnetpp-4.6/samples/inet/src/util/headerserializers/sctp -IC:/omnetpp-4.6/samples/inet/src/networklayer/arp -IC:/omnetpp-4.6/samples/inet/src/transport/tcp_common -IC:/omnetpp-4.6/samples/inet/src/transport/udp -IC:/omnetpp-4.6/samples/inet/src/mobility/common -LC:/omnetpp-4.6/samples/inet/out/$$\(CONFIGNAME\)/src -linet -DINET_IMPORT -KINET_PROJ=C:/omnetpp-4.6/samples/inet
#

# Name of target to be created (-o option)
TARGET = FirstMobilityProject$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I$(INET_PROJ)/src/linklayer/ieee80211/radio \
    -I$(INET_PROJ)/src/networklayer/routing/aodv \
    -I$(INET_PROJ)/src/networklayer/common \
    -I$(INET_PROJ)/src \
    -I$(INET_PROJ)/src/networklayer/icmpv6 \
    -I$(INET_PROJ)/src/world/obstacles \
    -I$(INET_PROJ)/src/networklayer/contract \
    -I$(INET_PROJ)/src/networklayer/xmipv6 \
    -I$(INET_PROJ)/src/networklayer/autorouting/ipv4 \
    -I$(INET_PROJ)/src/util \
    -I$(INET_PROJ)/src/transport/contract \
    -I$(INET_PROJ)/src/linklayer/common \
    -I$(INET_PROJ)/src/status \
    -I$(INET_PROJ)/src/linklayer/radio/propagation \
    -I$(INET_PROJ)/src/linklayer/ieee80211/radio/errormodel \
    -I$(INET_PROJ)/src/linklayer/radio \
    -I$(INET_PROJ)/src/util/headerserializers/tcp \
    -I$(INET_PROJ)/src/networklayer/ipv4 \
    -I$(INET_PROJ)/src/mobility/contract \
    -I$(INET_PROJ)/src/util/headerserializers/ipv4 \
    -I$(INET_PROJ)/src/base \
    -I$(INET_PROJ)/src/util/headerserializers \
    -I$(INET_PROJ)/src/world/radio \
    -I$(INET_PROJ)/src/linklayer/ieee80211/mac \
    -I$(INET_PROJ)/src/networklayer/ipv6 \
    -I$(INET_PROJ)/src/transport/sctp \
    -I$(INET_PROJ)/src/util/headerserializers/udp \
    -I$(INET_PROJ)/src/networklayer/ipv6tunneling \
    -I$(INET_PROJ)/src/util/headerserializers/ipv6 \
    -I$(INET_PROJ)/src/battery/models \
    -I$(INET_PROJ)/src/applications/pingapp \
    -I$(INET_PROJ)/src/linklayer/contract \
    -I$(INET_PROJ)/src/util/headerserializers/sctp \
    -I$(INET_PROJ)/src/networklayer/arp \
    -I$(INET_PROJ)/src/transport/tcp_common \
    -I$(INET_PROJ)/src/transport/udp \
    -I$(INET_PROJ)/src/mobility/common \
    -I. \
    -Ihotspotfiles \
    -Iresults \
    -Isrc \
    -Isrc/hotspots

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L$(INET_PROJ)/out/$(CONFIGNAME)/src  -linet
LIBS += -Wl,-rpath,`abspath $(INET_PROJ)/out/$(CONFIGNAME)/src`

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cpp and .msg files
OBJS = $O/src/DevelopmentHelper.o $O/src/LeviStatic.o $O/src/LevyMobility.o $O/src/hotspots/HotSpotsAlgorithm.o

# Message files
MSGFILES =

# Other makefile variables (-K)
INET_PROJ=C:/omnetpp-4.6/samples/inet

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) -DINET_IMPORT $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(Q)$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cpp

$O/%.o: %.cpp $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cpp %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cpp $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	$(qecho) Cleaning...
	$(Q)-rm -rf $O
	$(Q)-rm -f FirstMobilityProject FirstMobilityProject.exe libFirstMobilityProject.so libFirstMobilityProject.a libFirstMobilityProject.dll libFirstMobilityProject.dylib
	$(Q)-rm -f ./*_m.cpp ./*_m.h
	$(Q)-rm -f hotspotfiles/*_m.cpp hotspotfiles/*_m.h
	$(Q)-rm -f results/*_m.cpp results/*_m.h
	$(Q)-rm -f src/*_m.cpp src/*_m.h
	$(Q)-rm -f src/hotspots/*_m.cpp src/hotspots/*_m.h

cleanall: clean
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cpp hotspotfiles/*.cpp results/*.cpp src/*.cpp src/hotspots/*.cpp

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/src/DevelopmentHelper.o: src/DevelopmentHelper.cpp \
	src/DevelopmentHelper.h
$O/src/LeviStatic.o: src/LeviStatic.cpp \
	src/LeviStatic.h
$O/src/LevyMobility.o: src/LevyMobility.cpp \
	src/DevelopmentHelper.h \
	src/LeviStatic.h \
	src/LevyMobility.h \
	src/hotspots/HotSpot.h \
	src/hotspots/HotSpotsAlgorithm.h \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/Coord.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/base/ModuleAccess.h \
	$(INET_PROJ)/src/mobility/common/LineSegmentsMobilityBase.h \
	$(INET_PROJ)/src/mobility/common/MobilityBase.h \
	$(INET_PROJ)/src/mobility/common/MovingMobilityBase.h \
	$(INET_PROJ)/src/mobility/contract/IMobility.h \
	$(INET_PROJ)/src/util/FWMath.h
$O/src/hotspots/HotSpotsAlgorithm.o: src/hotspots/HotSpotsAlgorithm.cpp \
	src/DevelopmentHelper.h \
	src/LeviStatic.h \
	src/LevyMobility.h \
	src/hotspots/HotSpot.h \
	src/hotspots/HotSpotsAlgorithm.h \
	$(INET_PROJ)/src/base/Compat.h \
	$(INET_PROJ)/src/base/Coord.h \
	$(INET_PROJ)/src/base/INETDefs.h \
	$(INET_PROJ)/src/base/ModuleAccess.h \
	$(INET_PROJ)/src/mobility/common/LineSegmentsMobilityBase.h \
	$(INET_PROJ)/src/mobility/common/MobilityBase.h \
	$(INET_PROJ)/src/mobility/common/MovingMobilityBase.h \
	$(INET_PROJ)/src/mobility/contract/IMobility.h \
	$(INET_PROJ)/src/util/FWMath.h

