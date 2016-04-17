INSTALLS = cursors 

cursors.target = .
cursors.commands = cp *.png $(INSTALL_ROOT)/themes/default/cursors
cursors.path = /themes/default/cursors

CONFIG += warn_on

TEMPLATE = subdirs
