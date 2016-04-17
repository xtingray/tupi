INSTALLS = images 

images.target = .
images.commands = cp *.png $(INSTALL_ROOT)/themes/dark/images
images.path = /themes/dark/images

CONFIG += warn_on

TEMPLATE = subdirs
