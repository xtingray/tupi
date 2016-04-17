INSTALLS = images 

images.target = .
images.commands = cp *.png $(INSTALL_ROOT)/themes/default/images
images.path = /themes/default/images

CONFIG += warn_on
TEMPLATE = subdirs
