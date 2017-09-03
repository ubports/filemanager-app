#backup
schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- cp /var/lib/dpkg/info/python2.7-minimal.postinst /var/lib/dpkg/info/python2.7-minimal.postinst.bck
schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- cp /var/lib/dpkg/info/python-minimal.postinst /var/lib/dpkg/info/python-minimal.postinst.bck
schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- cp /var/lib/dpkg/info/python2.7.postinst /var/lib/dpkg/info/python2.7.postinst.bck

#remove
schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- rm /var/lib/dpkg/info/python2.7-minimal.postinst
schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- rm /var/lib/dpkg/info/python-minimal.postinst
schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- rm /var/lib/dpkg/info/python2.7.postinst


#restore
#schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- cp /var/lib/dpkg/info/python2.7-minimal.postinst.bck /var/lib/dpkg/info/python2.7-minimal.postinst
#schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- cp /var/lib/dpkg/info/python-minimal.postinst.bck /var/lib/dpkg/info/python-minimal.postinst
#schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- cp /var/lib/dpkg/info/python2.7.postinst.bck /var/lib/dpkg/info/python2.7.postinst


#configure
#schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- sh -x /var/lib/dpkg/info/python2.7-minimal.postinst configure 2.7.9-2ubuntu3
#schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- dpkg --configure -D 777 python2.7-minimal
#schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- dpkg --configure -D 777 python-minimal
#schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- dpkg --configure -D 777 python2.7

# install
schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- apt-get install python2.7-minimal:armhf python2.7:armhf python-minimal:armhf -y
schroot -u root -c source:click-ubuntu-sdk-15.04-armhf -- apt-get install libsmbclient-dev:armhf -y
