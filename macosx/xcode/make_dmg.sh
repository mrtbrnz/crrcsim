# make_dmg.sh shell script
#   This shell permits to build a compressed image disk.
#   This disk contains crrcsim application and readme.txt file.
#   crrcsim.dmg.tgz file is then ready to be used/uploaded as is.
#
mkdir /tmp/folder
mkdir /tmp/folder/disk
cp $SOURCE_ROOT/../README.txt /tmp/folder/disk/crrcsim.txt
cp $SOURCE_ROOT/README.txt /tmp/folder/disk/readme.txt
cp -R $TARGET_BUILD_DIR/crrcsim.app /tmp/folder/disk
hdiutil create -fs HFS+ -volname crrcsim -srcfolder /tmp/folder/disk /tmp/folder/crrcsim.dmg
cd /tmp/folder
tar -z -c -f crrcsim.dmg.tgz crrcsim.dmg
#cp crrcsim.dmg.tgz $HOME/Desktop
cp crrcsim.dmg.tgz $TARGET_BUILD_DIR
cd /tmp
rm -r folder
