.RECIPEPREFIX = >

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic
CFLAGS += -g
# CFLAGS += -Werror

LDFLAGS = -lraylib

CVECTOR_LIB_SRC_DIR = src/cvector
CVECTOR_LIB_STATIC = libcvector.a

SRC_DIR_PREFIX = src
APP_SRC_DIRS = $(SRC_DIR_PREFIX)/parser
# APP_SRC_DIRS += $(SRC_DIR_PREFIX)/raygui_impl
APP_SRC_DIRS += $(SRC_DIR_PREFIX)/viewer
APP_SRC_DIRS += $(SRC_DIR_PREFIX)/model

APP_SRC = $(foreach src_dir, $(APP_SRC_DIRS), $(wildcard $(src_dir)/*.c))
APP_OBJ = $(APP_SRC:.c=.o) 
APP_NAME = kitty_paws


##############   TARGET SECTION   ################
$(APP_NAME): $(APP_OBJ) $(CVECTOR_LIB_STATIC)
> $(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@
##############   END TARGET SECTION   ############


##############   DEPENDENCY SECTION   ################
$(CVECTOR_LIB_STATIC):
> $(MAKE) lib_static -C $(CVECTOR_LIB_SRC_DIR)
> cp $(CVECTOR_LIB_SRC_DIR)/$(CVECTOR_LIB_STATIC) .
##############   END DEPENDENCY SECTION   ############


##############   CLEAN SECTION   ################
clean_libs:
> $(RM) $(CVECTOR_LIB_STATIC)

clean_cvector:
> $(MAKE) clean -C $(CVECTOR_LIB_SRC_DIR)

clean_artifacts:
> $(RM) $(APP_OBJ)

clean: clean_libs clean_cvector clean_artifacts
> $(RM) $(APP_NAME)
##############   END CLEAN SECTION   ############
