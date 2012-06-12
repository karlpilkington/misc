#!/bin/bash
set -x

if [ -f /sbin/chkconfig ]
then 
#
  echo centos
  cp centos-exampled /etc/rc.d/init.d/exampled
  cp exampled /usr/bin/exampled
  /sbin/chkconfig --add exampled
  /etc/init.d/exampled start
#
elif [ -d /etc/init ]
then 
#
  echo ubuntu
  cp ubuntu-exampled.conf /etc/init/exampled.conf
  cp exampled /usr/bin/exampled
  start exampled
#
elif [ -d /Library/LaunchDaemons ]
then 
#
  echo mac
  cp mac-exampled.plist /Library/LaunchDaemons/exampled.plist
  cp exampled /usr/bin/exampled
  launchctl load /Library/LaunchDaemons/exampled.plist
#
else 
  echo unknown
fi

set +x
