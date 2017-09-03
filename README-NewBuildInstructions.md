Building a click package
========================

### For a device

For all the following instruction you need to be in the root directory of the clone repo (where also this file is)!

1. Be sure you have installed the Ubuntu SDK.
2. If not already, create a new 15.04 armhf chroot by running this command:
      sudo click chroot -a armhf -f ubuntu-sdk-15.04 create
3. If not already, copy the samba-3.0 to the chroot by running this script as root:
      sudo ./copy_samba_includes.sh
   Then the files should be in /usr/include/samba-3.0 directory (of your chroot!)
4. If not already, install the libsmbclient deps by running this script:
      ./fix_chroot_nonsense.sh
   (Thanks to Alan Pope!)
5. If not already, install the remaining deps by running this script:
      ./install_remaining_deps.sh
6. Now you need get the click deps by running this script:
      ./get_armhf_click_deps.sh
   (See get-click-deps file for alternative instructions)
7. Finally to build the click package you need to run this command:
      click-buddy --arch armhf --framework ubuntu-sdk-15.04
   (requires phablet-tools if not already installed on your machine)
8. Push the newly created click package onto your device and install it.
   (see README-Developers.md in the 'Installing a click package' caption)

Just repeat point 7 and 8, if you've done some modifications and you want to test them on your device.