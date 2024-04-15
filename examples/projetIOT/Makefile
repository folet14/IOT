# name of the application
APPLICATION = projetIOT

DevEUI?=cafebabe851a5307
AppKey?=CDD2D34304B66640669B196B1F8024B9
# If no BOARD is found in the environment, use this default:
BOARD ?= wyres-base
EXTERNAL_BOARD_DIRS=~/github/campusiot/RIOT-wyres/boards

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# required modules
USEMODULE += xtimer
USEMODULE += lps22hb
# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Fix the problem of the size of the debugging files
CFLAGS_DBG = 

include $(RIOTBASE)/tests/Makefile.tests_common
BOARD_WITHOUT_LORAMAC_RX := \
    arduino-mega2560 \
    i-nucleo-lrwan1 \
    stm32f0discovery \
    waspmote-pro \

LORA_DRIVER ?= sx1272
LORA_REGION ?= EU868

USEPKG += semtech-loramac
USEMODULE += $(LORA_DRIVER)

# load loramac RX if board supports it
ifeq (,$(filter $(BOARD),$(BOARD_WITHOUT_LORAMAC_RX)))
  USEMODULE += semtech_loramac_rx
endif

USEMODULE += auto_init_loramac
USEMODULE += shell_cmds_default
USEMODULE += fmt

FEATURES_OPTIONAL += periph_eeprom

USEPKG += cayenne-lpp

include $(RIOTBASE)/Makefile.include
