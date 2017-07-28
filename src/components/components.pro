CONFIG += ordered warn_on
TEMPLATE = subdirs

SUBDIRS += scenes \
           colorpalette \
           library \
           timeline \
           exposure \
           export \
           import \
           # kinas \
           brush \
           workspace \
           twitter \
           player 
  
# linux-g++ {
#     SUBDIRS += debug
# }
