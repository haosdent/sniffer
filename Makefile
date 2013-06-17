#
# PF_RING
#
PFRINGDIR  = /usr/src/PF_RING/userland/lib
LIBPFRING  = ${PFRINGDIR}/libpfring.a

#
# DNA Support
DNA_DEFINE = #-DENABLE_DNA_SUPPORT

#
# PF_RING aware libpcap
#
O_FLAG     = -O2 -DHAVE_PF_RING
PCAPDIR    = /usr/src/PF_RING/userland/libpcap-1.1.1-ring

#
# Search directories
#
PFRING_KERNEL=/usr/src/PF_RING/kernel
INCLUDE    = -I${PFRING_KERNEL} -I${PFRING_KERNEL}/plugins -I${PFRINGDIR} -I${PCAPDIR}

#
# C compiler and flags
#
#
# CROSS_COMPILE=arm-mv5sft-linux-gnueabi-
#
CC         = ${CROSS_COMPILE}gcc #--platform=native
CFLAGS     = -march=corei7-avx -mtune=corei7-avx  ${O_FLAG} -Wall ${INCLUDE} ${DNA_DEFINE} -D HAVE_ZERO -O2 # -g
# LDFLAGS  =

#
# User and System libraries
#
LIBTOOLS    = tools.o
LIBS       = ${LIBPFRING} ${LIBTOOLS} -lpthread -L/usr/src/PF_RING/userland/libpcap -lpcap -lrt

# How to make an object file
%.o: %.c pfutils.c
#	@echo "=*= making object $@ =*="
	${CC} ${CFLAGS} -c $< -o $@

#
# Main targets
#
TARGETS   = sniffer_pfring sniffer_socket blank_control

all: ${TARGETS}

sniffer_pfring: sniffer_pfring.o ${LIBPFRING} ${LIBTOOLS}
	${CC} ${CFLAGS} sniffer_pfring.o ${LIBS} -o $@

sniffer_socket: sniffer_socket.o ${LIBPFRING} ${LIBTOOLS}
	${CC} ${CFLAGS} sniffer_socket.o ${LIBS} -o $@

blank_control: blank_control.o ${LIBPFRING} ${LIBTOOLS}
	${CC} ${CFLAGS} blank_control.o ${LIBS} -o $@

clean:
	@rm -f ${TARGETS} *.o *~
