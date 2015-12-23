ReadMe - Run Autopilot tests
============================

# Prerequisites
* ubuntu-ui-toolkit-autopilot : sudo apt-get install ubuntu-ui-toolkit-autopilot

# Next Steps
Once the app has been built, you can go to the build directory and run:

    cd tests/autopilot
    autopilot3 run filemanager

You can run filemanager in phone or tablet mode passing one of these two switches:

-p for phone mode
-t for tablet mode

* autopilot3 launch ../../src/app/filemanager -p will launch filemanager with autopilot in phone mode
* autopilot3 launch ../../src/app/filemanager -t will launch filemanager with autopilot in tablet mode.
