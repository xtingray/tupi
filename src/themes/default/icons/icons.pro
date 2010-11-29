# Subdir relative project main directory: ./src/themes/default/icons
# Target is a library:  

INSTALLS = icons 

icons.target = .
icons.commands = cp *.png $(INSTALL_ROOT)/themes/default/icons
icons.path = /themes/default/icons

CONFIG += release \
          warn_on \
          staticlib 

TEMPLATE = lib

# little hack
macx{
    TEMPLATE = subdirs
}

