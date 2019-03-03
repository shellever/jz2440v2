tar -jxvf yaffs_source_util_larger_small_page_nand.tar.bz2

cd Development_util_ok/yaffs2/utils
make

cp -p mkyaffs2image mkyaffsimage ../../../../

cd -
rm -rf Development_util_ok


# output
# mkyaffs2image
# mkyaffsimage
