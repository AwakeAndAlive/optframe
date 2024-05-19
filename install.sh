#!/bin/bash

echo "Installing OptFrame (with OptFCore) into /home/augusto01/.local/include and /home/augusto01/.local/optframe"

if [[ ! $(echo 0) ]]; then exit; fi

read -r -p "Continue? y/n " input

case $input in
    [yY][eE][sS]|[yY])
    echo "Yes"

    cp -r include/OptFrame /home/augusto01/.local/include/
    cp -r include/OptFCore /home/augusto01/.local/include/

    mkdir -p /home/augusto01/.local/optframe

    cp -r Examples /home/augusto01/.local/optframe/
    cp -r src      /home/augusto01/.local/optframe/
    cp -r tests    /home/augusto01/.local/optframe/
    cp    makefile /home/augusto01/.local/optframe/
    echo "Finished installation!"
 ;;
    [nN][oO]|[nN])
    echo "No"
    echo "Aborting..."
    exit 1
       ;;
    *)
 echo "Invalid input!"
 echo "Aborting..."
 exit 1
 ;;
esac