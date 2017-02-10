CONFIG += ordered warn_on
TEMPLATE = subdirs

SUBDIRS += scenes \
           colorpalette \
           library \
           timeline \
           exposure \
           export \
           import \
           help \
           # kinas \
           brush \
           paintarea \
           twitter \
           animation 
		   
linux-g++ {
    SUBDIRS += debug
}
