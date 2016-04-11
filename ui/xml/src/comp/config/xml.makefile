# XML managed build extension makefile fragment.
# Incorporates the XML build into the managed build of Wind River Workbench.

export WIND_HOME_FS := $(subst \,/,$(WIND_HOME))
WEBSERVICES_BASE = $(shell sh $(WIND_HOME_FS)/wrenv.sh -o print_path '$$(WIND_COMPONENTS)' $(COMP_WEBSERVICES))
export WEBSERVICES_BASE := $(subst \,/,$(WEBSERVICES_BASE))
export COMPONENTS_DIR := $(patsubst %/$(COMP_WEBSERVICES),%,$(WEBSERVICES_BASE))
export COMP_WEBSERVICES_PATH := $(WEBSERVICES_BASE)
SPACE := krnl
ifeq ($(PROJECT_TYPE),RTP)
	SPACE := usr
endif
ifeq ($(PROJECT_TYPE),SL)
	SPACE := usr
endif
export ADDED_INCLUDES += -I$(COMP_WEBSERVICES_PATH)/h
ifeq ($(VSB_DIR),)
ifeq ($(findstring SMP,$(VXBUILD)),SMP)
    export ADDED_LIBPATH += -L$(COMPONENTS_DIR)/obj/$(WIND_PLATFORM)/$(SPACE)/lib_smp/$(VX_CPU_FAMILY)/$(CPU)/$(subst $(TOOL_FAMILY),common,$(TOOL))
else
    export ADDED_LIBPATH += -L$(COMPONENTS_DIR)/obj/$(WIND_PLATFORM)/$(SPACE)/lib/$(VX_CPU_FAMILY)/$(CPU)/$(subst $(TOOL_FAMILY),common,$(TOOL))
endif
else
    export ADDED_LIBPATH += -L$(VSB_DIR)/$(VX_CPU_FAMILY)/$(CPU)/$(subst $(TOOL_FAMILY),common,$(TOOL))
endif
export ADDED_LIBS += -lxml
