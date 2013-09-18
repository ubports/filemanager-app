all:

run:
	/usr/bin/qmlscene $@ ubuntu-filemanager-app.qml

icon:
	inkscape --export-png=icons/$(ICON).png --export-dpi=32 --export-background-opacity=0 --without-gui /usr/share/icons/ubuntu-mobile/actions/scalable/$(ICON).svg

