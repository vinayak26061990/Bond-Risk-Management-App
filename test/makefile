# makefile to build library and testdriver
.SUFFIXES:.cc
.cc.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# assign your compiler name to CC if not g++
CC = g++
# where your header files are (. is same directory as the makefile)
INCLUDES=-I.
# turn on debugging symbols
CFLAGS=-g
# where your object files are (. is the same directory as the makefile)
LDFLAGS=-L.

# when "make" is typed at command line with no args "default" will get executed and do "all"
default: 	all

#
# assign source and headers for our "utility" lib to variables below
#
# replace "SBB" (my initials) with yours or use the same filenames
LIB_DATE_SRC = SBB_date.cc
LIB_DATE_SRC_HDR = SBB_date.h
LIB_IO_SRC=SBB_io.cc
LIB_IO_SRC_HDR=SBB_io.h
LIB_YTM_SRC = Instrument.cc 
LIB_YTM_SRC_HDR = Instrument.h 
LIB_YC_SRC = Yieldcurve.cc 
LIB_YC_SRC_HDR = Yieldcurve.h 
LIB_RATING_SRC=SBB_ratings.cc
LIB_RATING_SRC_HDR=SBB_ratings.h
LIB_HEDGING_SRC=Hedging.cc
LIB_HEDGING_SRC_HDR=Hedging.h
LIB_VAR_SRC=Var.cc
LIB_VAR_SRC_HDR=Var.h

#
LIB_UTIL_TARGET = libutil.a
LIB_DATE_OBJ = $(LIB_DATE_SRC:.cc=.o)
$(LIB_DATE_OBJ) : $(LIB_DATE_SRC_HDR)

LIB_IO_OBJ = $(LIB_IO_SRC:.cc=.o)
$(LIB_IO_OBJ) : $(LIB_IO_SRC_HDR)

LIB_YTM_OBJ = $(LIB_YTM_SRC:.cc=.o)  
$(LIB_YTM_OBJ) : $(LIB_YTM_SRC_HDR)

LIB_YC_OBJ = $(LIB_YC_SRC:.cc=.o)  
$(LIB_YC_OBJ) : $(LIB_YC_SRC_HDR)

LIB_RATING_OBJ = $(LIB_RATING_SRC:.cc=.o)  
$(LIB_RATING_OBJ) : $(LIB_RATING_SRC_HDR)

LIB_HEDGING_OBJ = $(LIB_HEDGING_SRC:.cc=.o)  
$(LIB_HEDGING_OBJ) : $(LIB_HEDGING_SRC_HDR)

LIB_VAR_OBJ = $(LIB_VAR_SRC:.cc=.o)  
$(LIB_VAR_OBJ) : $(LIB_VAR_SRC_HDR)
# 
# the file where main() is
MAIN_SRC = mmain.cc
MAINOBJ = $(MAIN_SRC:.cc=.o) 
LIB_SERVER_SRC=sbb_server.c
LIBSERVEROBJ= $(LIB_SERVER_SRC:.c=.o)
LIB_CLIENT_SRC=sbb_client.c
LIBCLIENTOBJ= $(LIB_CLIENT_SRC:.c=.o)
$(LIBSERVEROBJ) : $(LIB_UTIL_TARGET)
MAINTARGET=testdriver
CLIENTTARGET=clientdriver

ARCHIVE_OPTIONS = rucsv

$(LIB_UTIL_TARGET) :  $(LIB_DATE_OBJ) $(LIB_IO_OBJ) $(LIB_YTM_OBJ) $(LIB_YC_OBJ) $(LIB_RATING_OBJ) $(LIB_HEDGING_OBJ) $(LIB_VAR_OBJ)
	ar $(ARCHIVE_OPTIONS) $(LIB_UTIL_TARGET)  $(LIB_DATE_OBJ) $(LIB_IO_OBJ) $(LIB_YTM_OBJ) $(LIB_YC_OBJ) $(LIB_RATING_OBJ) $(LIB_HEDGING_OBJ) $(LIB_VAR_OBJ)

$(CLIENTTARGET) : $(LIBCLIENTOBJ) 
	$(CC)  $(LDFLAGS) $(LIBCLIENTOBJ) -o $(CLIENTTARGET)

$(MAINTARGET) : $(LIBSERVEROBJ) $(LIB_UTIL_TARGET)
	$(CC)  $(LDFLAGS) $(LIBSERVEROBJ) $(LIB_UTIL_TARGET)  -o $(MAINTARGET)



all : util test

test: $(MAINTARGET)

util: $(LIB_UTIL_TARGET)


clean:
	rm -f *.o 

clobber:
	-rm -f *.o *.a $(MAINTARGET)

backup: clobber
	tar cvf ./nyu.tar *.*
