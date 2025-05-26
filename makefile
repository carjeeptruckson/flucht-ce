# ----------------------------
# Makefile Options
# ----------------------------

NAME = FLUCHT
ICON = icon.png
DESCRIPTION = "Wall jump as high as you can!"
COMPRESSED = YES

# Standard CFLAGS and CXXFLAGS
CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# Using the OS's limited printf for smaller binary size
HAS_PRINTF = NO

# Directory where convimg.yaml and image assets are located
# SRCDIR is usually 'src', so GFXDIR becomes 'src/sprites'
GFXDIR = $(SRCDIR)/sprites

# The main generated C header file by convimg
GFX_HEADER = $(GFXDIR)/gfx.h
# The main generated C source file by convimg
GFX_SOURCE = $(GFXDIR)/gfx.c

# DEPS tells the main makefile about additional targets that might need to be built.
DEPS = $(GFX_HEADER) $(GFX_SOURCE)

# ----------------------------
# Include the main CEdev makefile
# This should be AFTER your custom variable definitions and DEPS.
# ----------------------------
include $(shell cedev-config --makefile)

# ----------------------------
# Custom Rules
# ----------------------------

# This rule explicitly tells 'make' how to build the generated graphics C files.
# It states that gfx.h and gfx.c depend on the convimg.yaml file.
# If gfx.h or gfx.c are missing, or if convimg.yaml is newer,
# this rule's command will be executed.
$(GFX_HEADER) $(GFX_SOURCE): $(GFXDIR)/convimg.yaml
	@echo "[gfx] Generating C sources from $(GFXDIR)/convimg.yaml..."
	$(Q)$(MAKE_GFX_CMD)

# If MAKE_GFX_CMD is not defined for some reason in your toolchain version,
# you can fallback to the more explicit command:
# $(GFX_HEADER) $(GFX_SOURCE): $(GFXDIR)/convimg.yaml
#	@echo "[gfx] Generating C sources from $(GFXDIR)/convimg.yaml..."
#	$(Q)cd $(GFXDIR) && $(CONVIMG)