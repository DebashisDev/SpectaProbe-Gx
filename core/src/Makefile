#########################################################################
#																		#
# SCRIPT NAME	: Makefile												#
# DESCRIPTION	: To build all .cpp files								#
# DATE 			: 19-02-2016										    #
# AUTHOR		: Debashis.											    #
#																		#
# Copyright (c) 2016, Pinnacle Digital (P) Ltd. New-Delhi				# 
#########################################################################

include ${PROBE_ROOT}/probe.mk
OBJS= \
	AdminPortReader.o 	\
	BaseConfig.o 		\
	BWData.o 			\
	DiameterGlobal.o	\
	EthernetParser.o 	\
	GConfig.o 			\
	GConstant.o 		\
	glbTimer.o			\
	IpAddress.o 		\
	IPGlobal.o 			\
	Main.o 				\
	PacketRouter.o 		\
	ProbeStats.o 		\
	ProbeStatsLog.o 	\
	ProbeUtility.o 		\
	SpectaProbe.o

all:${OBJS}

.cpp.o:

	${GCC} ${CFLAGS} -I/usr/include ${PROBE_INCLUDE} -c $< 

clean:
	${RM} ${OBJS}
