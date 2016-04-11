# SOAP managed build extension makefile fragment.
# Incorporates the SOAP build into the managed build of Wind River Workbench.

export WIND_HOME_FS := $(subst \,/,$(WIND_HOME))
WEBSERVICES_BASE = $(shell sh $(WIND_HOME_FS)/wrenv.sh -o print_path '$$(WIND_COMPONENTS)' $(COMP_WEBSERVICES))
export WEBSERVICES_BASE := $(subst \,/,$(WEBSERVICES_BASE))
export COMPONENTS_DIR := $(patsubst %/$(COMP_WEBSERVICES),%,$(WEBSERVICES_BASE))
export COMP_WEBSERVICES_PATH := $(WEBSERVICES_BASE)
export ADDED_CFLAGS += -DWITH_LEANER
export ADDED_C++FLAGS += -DWITH_LEANER
export ADDED_INCLUDES += -I$(COMP_WEBSERVICES_PATH)/h \
	 -I$(COMP_WEBSERVICES_PATH)/h/webservices/soap \
	 -I$(COMP_WEBSERVICES_PATH)/h/webservices/soap/plugin
