INSTALLS = config 

config.target = .
config.commands = cp *.qss $(INSTALL_ROOT)/themes/dark/config
config.path = /themes/dark/config

CONFIG += warn_on

TEMPLATE = subdirs
