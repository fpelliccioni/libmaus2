#! /bin/bash

# update branches
git checkout experimental
git pull -v
git checkout experimental-debian
git pull -v
git checkout debian
git pull -v
git checkout master
git pull -v

# merge
git checkout master
git merge -m "Merge experimental branch into master branch" experimental
git push -v

git checkout debian
git merge -m "Merge experimental-debian branch into debian branch" experimental-debian
git push -v

# add release tag/branch
git checkout master
VERSION=`grep <configure.ac "AC_INIT" | perl -p -e "s/.*AC_INIT\(//" | awk -F ',' '{print $2}'`
DATE=`date +"%Y%m%d%H%M%S"`
RELEASE=${VERSION}-release-${DATE}
git checkout -b ${RELEASE}-branch master
PATH=/software/hpag/autotools/bin:${PATH} autoreconf -i -f
ADDFILES="INSTALL Makefile.in aclocal.m4 autom4te.cache compile config.guess config.h.in config.sub configure depcomp install-sh ltmain.sh m4/libtool.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4 m4/lt~obsolete.m4 missing src/Makefile.in test/Makefile.in"
mv .gitignore .gitignore_
git add ${ADDFILES}
git commit -m "Release ${RELEASE}"
mv .gitignore_ .gitignore
git tag ${RELEASE}
git push -v origin ${RELEASE}
git checkout master
git branch -D ${RELEASE}-branch
git checkout experimental

# launchpad branch update
git checkout debian
tar czvf debian.tar.gz debian
git checkout debian-launchpad
git merge -m "Merge master branch into debian branch" master
tar xzvf debian.tar.gz
rm debian.tar.gz
sed -i -e  "s|libstaden-read-dev (>= 1\.13\.0)|staden-io-lib-trunk-dev (>= 1.13.0)|" debian/control
git commit -a -m "update dependency for launchpad"
git push -v
git checkout experimental

rm -fR debian
