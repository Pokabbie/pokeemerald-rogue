#!/bin/sh

echo "Gathering dependencies:"

echo "Poryscript.."
if [ -d "tools/poryscript" ] 
then
  echo "Skipping (Already exists)"
else
  mkdir -p tools/poryscript
  wget -O tools/poryscript/poryscript-linux.zip https://github.com/huderlem/poryscript/releases/download/3.0.2/poryscript-linux.zip
  wget -O tools/poryscript/poryscript-windows.zip https://github.com/huderlem/poryscript/releases/download/3.0.2/poryscript-windows.zip
  unzip tools/poryscript/poryscript-linux.zip -d tools/poryscript
  unzip tools/poryscript/poryscript-windows.zip -d tools/poryscript
  echo "Done."
fi

echo "Gathering dependencies finished."