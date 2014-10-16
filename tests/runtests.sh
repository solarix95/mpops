#!/bin/bash

rm -rf out*

cnt=1
while read nextCmd
do
  echo "--------------------------- TESTRUN ----------------------------"
  echo "--------------------------- $cnt"
  $nextCmd
  let cnt=${cnt}+1
done < test.cmds

echo "--------------------------- ALL DONE ----------------------------"
ls out*
