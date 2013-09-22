#! /bin/sh

#
# create source tarball
#

APPNAME=crrcsim

if [ $# -ne 1 ]
then
  echo 1>&2 Wrong argument list: $*
  echo 1>&2 Usage: $0 REVISION
  echo 1>&2 This script does only take one argument: the revision to use. Examples:
  echo 1>&2 "   tip             # to get the most current version"
  echo 1>&2 "   0.9.10          # if you used 'hg tag 0.9.10' before"
  echo 1>&2 "   \"Release 0.9.9\" # if you used 'hg tag \"Release 0.9.9\"' before"
  echo 1>&2 "   1043            # hg repo version number"
  echo 1>&2 "   ab34a48fe       # hg repo version hash"
  echo 1>&2
  exit -1
fi

RD=`hg root`
cd ${RD}

# define a good label
TAG=`hg identify -t -r "$1"`
REVID=`hg identify -i -r "$1"`
DATE=`hg log --template "{date|shortdate}" -r "$1"`
REVNAME=${TAG}

if test -z "${TAG}"
  then
    TAG=${DATE}_${REVID}
    REVNAME=${REVID}
  else
    if test "${TAG}" = "tip"
      then
        TAG=${DATE}_${REVID}
        REVNAME=${REVID}
    fi
fi
TAG=`echo ${TAG} | sed "s/[ \t]/_/g"`
echo "Revision label is: "${TAG}

# create changelog
LOGFILE=`mktemp`
hg glog -l 100 --template "--- {rev} --- {node|short} --- {date|isodate} --- {tags}\n    User: {author}\n\t{desc|strip|tabindent}\n\n" >> ${LOGFILE}

#
DSTDIRTMP=`mktemp -d`
DSTDIR="${DSTDIRTMP}/${APPNAME}-${TAG}"

# create files and dirs
hg archive -r "$1" ${DSTDIR}

cd ${DSTDIR}

##
## insert changelog/release notes
## see http://tldp.org/HOWTO/Software-Release-Practice-HOWTO/distpractice.html
##
CHGLOGNAME="./HISTORY"
echo "This is version ${REVNAME}, created on ${DATE}"        >  ${CHGLOGNAME}
echo "---------------------------------------------------" >> ${CHGLOGNAME}
echo ""                                                    >> ${CHGLOGNAME}
if test -f ./documentation/ReleaseNotes
  then
    cat ./documentation/ReleaseNotes >> ${CHGLOGNAME}
    rm  ./documentation/ReleaseNotes
fi
echo ""                                                    >> ${CHGLOGNAME}
echo "---------------------------------------------------" >> ${CHGLOGNAME}
echo "Detailed changelog:"                                 >> ${CHGLOGNAME}
echo ""                                                    >> ${CHGLOGNAME}
cat ${LOGFILE} >> ${CHGLOGNAME}

# remove some files
rm ${DSTDIR}/.hgignore
rm ${DSTDIR}/.hg_archival.txt

# update version string
if test -f ${DSTDIR}/CMakeLists.txt
  then
    sed -e "s/set(CMAKE_PACKAGE_VERSION \"dev\")/set(CMAKE_PACKAGE_VERSION \"${TAG}\")/" -i ${DSTDIR}/CMakeLists.txt
fi
sed -i "s/AC_INIT.*/AC_INIT(crrcsim, ${TAG}, crrcsim-devel@lists.berlios.de)/" configure.ac
sed -i "s/.define PRODUCT_VERSION .*/!define PRODUCT_VERSION \"${TAG}\"/" packages/Win32/crrcsim.nsi

# todo: update strings in packages/

##
## create archive
##

# this is a raw archive, without having run autotools:
cd ..
tar cfj ./crrcsim-${TAG}.tar.bz2 ./crrcsim-${TAG}/*

cd ./crrcsim-${TAG}
if test -f ./autogen.sh
  then
    # This is a version which does use autotools
    ./autogen.sh
    ./configure
    make distcheck
    # this creates a tar.gz, too!
fi

echo "Output is in "${DSTDIRTMP}" and "${DSTDIR}
