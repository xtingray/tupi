INSTALLS = icons 

icons.target = .
icons.commands = cp *.png $(INSTALL_ROOT)/themes/default/icons
icons.path = /themes/default/icons

CONFIG += warn_on

TEMPLATE = subdirs
