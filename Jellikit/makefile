###	BeOS Makefile ###

# specify the name of the binary
NAME= Jellikit

# specify the type of binary
#	APP:	Application
#	SHARED:	Shared library or add-on
#	STATIC:	Static library archive
#	DRIVER: Kernel Driver - not implemented yet
TYPE= APP

# specify the source files to use
SRCS=	AppResStrings.cpp \
	AttrApp.cpp \
	AttrWindow.cpp \
	EditorWindow.cpp \
	AddAttrWindow.cpp \
	AboutWindow.cpp \
	AboutView.cpp \
	AddonManager.cpp \
	AttributeManager.cpp\
	AddOnWrapper.cpp \
	AttrItem.cpp \
	sgb-source/CLVColumn.cpp \
	sgb-source/CLVColumnLabelView.cpp \
	sgb-source/CLVListItem.cpp \
	sgb-source/ColumnListView.cpp \
	sgb-source/MouseWatcher.cpp
	
		
# specify the resource files to use
RSRCS= Jellikit.rsrc

#specify additional libraries to link against
#	if libName.so or libName.a is the name of the library to link against
# 	then simply specify Name in the LIBS list
# 	if there is another naming scheme use the full binary
#	name: my_library.so or my_lib.a
LIBS =	be tracker media

#	specify the paths to directories where additional
# 	libraries are to be found.  /boot/develop/lib/PLATFORM/ is
#	already set.  The paths can be full or relative to this
#	makefile.  The paths included may not be recursive, so
#	specify all of the needed paths explicitly
LIBPATHS= /boot/develop/lib/$(CPU)

#	specify additional directories where header files can be found
# 	directories where sources are found are included automatically
INCPATHS= ./sgb-headers

#	specify the level of optimization that you desire
#	NONE, SOME, FULL
OPTIMIZE= 

#	specify any symbols to be defined.  The symbols will be
#	set to a value of 1.  For example specify DEBUG if you want
#	DEBUG=1 to be set when compiling.
DEFINES= 

#	specify special warning levels
#	if unspecified default warnings will be used
#	NONE = supress all warnings
#	ALL = enable all warnings
WARNINGS = ALL

#	specify symbols
#	if TRUE debug symbols will be created
SYMBOLS = 


##### Don't go beneath here  #####

BELIBFILES=glue-noinit.a;init_term_dyn.o;start_dyn.o;libroot.so;

#project independant information
MACHINE=$(shell uname -m)
ifeq ($(MACHINE), BePC)
	CPU = x86
else
	CPU = ppc
endif

#	set the object directory
	OBJ_DIR		:= obj.$(CPU)

# 	define the target
	TARGET		:= $(NAME)

#	specify the mimeset tool
	MIMESET		:= mimeset
#	specify the default libraries

# specify the resource tools
	XRES		= xres

# set the compiler flags to output symbols
ifeq ($(SYMBOLS), TRUE)
	CFLAGS += -g
endif


# platform specific settings
ifeq ($(CPU), x86)
#	set the compiler and compiler flags
	CC		=	gcc
	CFLAGS	+=	
#	LIBS	= $(X86LIBS)
	
#	set the proper optimization level
	ifeq ($(OPTIMIZE), FULL)
		OPTIMIZER	= -O3
	else
	ifeq ($(OPTIMIZE), SOME)
		OPTIMIZER	= -O1
	else
	ifeq ($(OPTIMIZE), NONE)
		OPTIMIZER	=
	endif
	endif
	endif

	CFLAGS += $(OPTIMIZER)
	
	ifeq ($(WARNINGS), ALL)
		CFLAGS += -Wall -Wno-multichar -Wno-ctor-dtor-privacy
	else
	ifeq ($(WARNINGS), NONE)
	
	endif
	endif


#	set the linker and linker flags
	LD			= gcc
	LDFLAGS		=

	ifeq ($(TYPE), APP)
	
	else
	ifeq ($(TYPE), SHARED)
		LDFLAGS += -addon -fpic -DT_SONAME=$(NAME)
	else
	ifeq ($(TYPE), STATIC)
	
	else
	ifeq ($(TYPE), DRIVER)
	
	endif 
	endif 
	endif 
	endif 

else
ifeq ($(CPU), ppc)
#	set the compiler and compiler flags
	CC		=	mwcc
	CFLAGS	+=	
#	LIBS 	=	$(PPCLIBS)
	
#	set the proper optimization level
	ifeq ($(OPTIMIZE), FULL)
		OPTIMIZER	= -O7
	else
	ifeq ($(OPTIMIZE), SOME)
		OPTIMIZER	= -O3
	else
	ifeq ($(OPTIMIZE), NONE)
		OPTIMIZER	=
	endif
	endif
	endif

	CFLAGS += $(OPTIMIZER)
	
	ifeq ($(WARNINGS), ALL)
		CFLAGS += -w all
	else
	ifeq ($(WARNINGS), NONE)
		CFLAGS += -w 0
	endif
	endif


#	set the linker and linker flags
	LD			= mwld
	LDFLAGS		=  \
					-export pragma \
					-init _init_routine_ \
					-term _term_routine_

	ifeq ($(TYPE), APP)
		LDFLAGS += 
	else
	ifeq ($(TYPE), SHARED)
		LDFLAGS += -xms
	else
	ifeq ($(TYPE), STATIC)
		LDFLAGS += -xml
	else
	ifeq ($(TYPE), DRIVER)
	
	endif 
	endif 
	endif 
	endif 

endif
endif

# psuedo-function for converting a list of source files in SRCS variable
# to a corresponding list of object files in $(OBJ_DIR)/xxx.o
# The "function" strips off the src file suffix (.ccp or .c or whatever)
# and then strips of the directory name, leaving just the root file name.
# It then appends the .o suffix and prepends the $(OBJ_DIR)/ path
define SRCS_LIST_TO_OBJS
	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(foreach file, $(SRCS), $(basename $(notdir $(file))))))
endef

OBJS = $(SRCS_LIST_TO_OBJS) 

# create a unique list of paths to our sources
SRC_PATHS += $(sort $(foreach file, $(SRCS), $(dir $(file))))

VPATH :=
# add the list of source paths to VPATH if not already present
VPATH += $(addprefix :, $(subst  ,:, $(filter-out $($(subst, :, ,$(VPATH))), $(SRC_PATHS))))

# add the list of source paths to INLCUDES if not already present
INCLUDES = $(foreach path, $(INCPATHS) $(SRC_PATHS), $(addprefix -I, $(path)))


# add the -L prefix to all library paths to search
LINK_PATHS = $(foreach path, $(LIBPATHS), $(addprefix -L, $(path)))

# add the -l prefix to all libs to be linked against
LINK_LIBS = $(foreach lib, $(LIBS), $(addprefix -l, $(lib)))

# add to the linker flags 
LDFLAGS += $(LINK_PATHS) $(LINK_LIBS)

#	add the defines to the compiler flags
CFLAGS += $(foreach define, $(DEFINES), $(addprefix -D, $(define)))

ifeq ($(TYPE), STATIC)
	BUILD_LINE = ar -cru $(NAME) $(OBJS)
else
	BUILD_LINE = $(LD) -o $@ $(OBJS) $(LDFLAGS)
endif

#	create the resource instruction
	ifeq ($(RSRCS), )
		DO_RSRCS :=
	else
		DO_RSRCS := $(XRES) -o $(TARGET) $(RSRCS)
	endif


#	define the actual work to be done	
default: $(TARGET)

$(TARGET):	$(OBJ_DIR) $(OBJS) $(RSRCS)
		$(BUILD_LINE)
		$(DO_RSRCS)
		$(MIMESET) -f $@


#	rule to create the object file directory if needed
$(OBJ_DIR)::
	@[ -d $(OBJ_DIR) ] || mkdir $(OBJ_DIR) > /dev/null 2>&1

$(OBJ_DIR)/%.o : %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o : %.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

#	empty rule. Things that depend on this rule will always get triggered
FORCE:

#	The generic clean command. Delete everything in the object folder.
clean :: FORCE
	-rm -rf $(OBJ_DIR)

#	remove just the application from the object folder
rmapp ::
	-rm -f $(TARGET)
