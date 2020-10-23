# ----------------------------
# Program Options
# ----------------------------

NAME         ?= SANSTEST
ICON         ?= icon.png
DESCRIPTION  ?= "SANS84"
COMPRESSED   ?= YES
ARCHIVED     ?= NO

# ----------------------------
# Debug Options
# ----------------------------
.DEFAULT_GOAL := debug
#OUTPUT_MAP   ?= YES


include $(CEDEV)/meta/makefile.mk
