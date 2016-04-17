INSTALLS = icons 

icons.target = .
icons.commands = cp *.png $(INSTALL_ROOT)/themes/dark/icons
icons.path = /themes/dark/icons

CONFIG += warn_on

TEMPLATE = subdirs
