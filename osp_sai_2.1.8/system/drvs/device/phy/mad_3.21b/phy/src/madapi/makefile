# Source files in this directory
TARGET = 
CSOURCES	= madSysConfig.c madSysCtrl.c madFbCtrl.c	   \
         madCpCtrl.c madCpStatus.c madMACCtrl.c  madSyncClkCtrl.c \
         madInt.c madPktGen.c madStats.c madVod.c madWol.c madIEEETest.c \
         madLoopback.c madXmdioRegCtrl.c madXmdioRegStatus.c madVersion.c \
         madPatGen.c madUSGMII.c madTemp.c madEEECtrl.c madSNR.c

ifeq (0,1)
#ifeq (1,1)
CSOURCES	+= dspVct/madDSPVct.c  
#ADDED_CFLAGS	+= -DMAD_DSP_VCT
endif

ASOURCES	=

CSOURCES	+=   madVctTdr.c  madVctAdv.c  madVctExt.c madVctAlt.c   
ifeq ($(VCT_PENDING),TRUE)
CSOURCES	+=   madSMVct.c
#ADDED_CFLAGS	+= -DMAD_SM_VCT
endif

# Include common variable definitions
ifeq ($(OS_RUN),VXWORKS)
include $(MAD_TOOL_DIR)\make.defs
endif
ifeq ($(OS_RUN),LINUX)
include $(MAD_TOOL_DIR)/makelnx.defs
endif

# Add in extra stuffs
EXTRA_INCLUDE	+=
EXTRA_DEFINE	+=
ADDED_CFLAGS	+=
#ADDED_CFLAGS	+= -DDEBUG_MAD

# Include common build rules
ifeq ($(OS_RUN),VXWORKS)
include $(MAD_TOOL_DIR)\make.rules
endif
ifeq ($(OS_RUN),LINUX)
include $(MAD_TOOL_DIR)/makelnx.rules
endif

# end of file
