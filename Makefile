
INSTALL_LIB_PATH=$(ROOT_PATH)/usr/local/lib
INSTALL_INC_PATH=$(ROOT_PATH)/usr/local/include/mrx/ai
PROJECT=mrxai
VERSION=0.0.1

#compiler
CXX = g++
CC = gcc
LDXX = g++
LD = gcc
AR = ar
LN = ln
CP = cp
MKDIR = mkdir
CD = cd
RM = rm
RSYNC = rsync

OUTPUT = lib$(PROJECT).so
MAIN_C_SOURCES=$(shell find src -name "*.c" | grep -v main.c)
MAIN_C_OBJECTS=$(MAIN_C_SOURCES:.c=.lo)

MAIN_CPP_SOURCES=$(shell find src -name "*.cpp" | grep -v main.cpp)
MAIN_CPP_OBJECTS=$(MAIN_CPP_SOURCES:.cpp=.lo)

MAIN_APP_OBJECTS=${MAIN_C_OBJECTS} ${MAIN_CPP_OBJECTS}

INCLUDE= -I. -Isrc -Isrc/ann -Isrc/bpn

HEADERS= src/bpn.h src/ann.h	\
	$(shell find src/bpn -name "*.h")	\
	$(shell find src/ann -name "*.h")

LINK= -lm

DEFINES= -D'VERSION_MAJOR=0' -D'VERSION_MINOR=1' -D'VERSION_REVISION=0' -D'VERSION_STRING="0.1.0"'

CPPFLAGS=

all : $(OUTPUT)

$(OUTPUT): ${MAIN_APP_OBJECTS}
	$(CROSS_COMPILE)$(CC) -shared ${MAIN_APP_OBJECTS} -o $(OUTPUT).$(VERSION)
	$(LN) -s $(OUTPUT).$(VERSION) $(OUTPUT)

%.lo : %.c
	$(CROSS_COMPILE)$(CC) -c -fPIC $< -o $@ $(INCLUDE) $(DEFINES)

%.lo : %.cpp
	$(CROSS_COMPILE)$(CXX) -c -fPIC $< -o $@ $(INCLUDE) $(DEFINES)

install-dev:

install:
	$(MKDIR) -p $(INSTALL_LIB_PATH)
	$(MKDIR) -p $(INSTALL_INC_PATH)
	$(CP) -f $(OUTPUT).$(VERSION) $(INSTALL_LIB_PATH)
	$(RSYNC) -r --exclude="*.c" --exclude="*.cpp" --exclude="*.lo" src/ $(INSTALL_INC_PATH)
	$(CD) $(INSTALL_LIB_PATH); $(RM) -f $(OUTPUT);$(LN) -s $(OUTPUT).$(VERSION) $(OUTPUT)

uninstall:
	$(RM) -f $(INSTALL_LIB_PATH)/$(OUTPUT).$(VERSION) $(INSTALL_LIB_PATH)/$(OUTPUT)
	$(CD) $(INSTALL_INC_PATH); $(RM) -rf bpn.h ann.h bpn ann

clean:
	rm -f ${MAIN_APP_OBJECTS} $(OUTPUT) $(OUTPUT).$(VERSION)
	
