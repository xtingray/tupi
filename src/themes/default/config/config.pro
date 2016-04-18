INSTALLS = config 

config.target = .
config.commands = cp *.qss $(INSTALL_ROOT)/themes/default/config
config.path = /themes/default/config

CONFIG += warn_on

TEMPLATE = subdirs
