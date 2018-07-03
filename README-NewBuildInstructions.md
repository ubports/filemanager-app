Building a click package
========================

### For a device with xenial

1. Be sure you have installed latest [Clickable tool](https://github.com/bhdouglass/clickable)

2. Clone the current branch
```
git clone https://github.com/ubports/filemanager-app.git [-b *branchname*]
```

3. Go to the cloned directory
```
cd filemanager-app
```
   
4. Run clickable
```
clickable
```
   
5. If you have your device connected via adb, the click packages automatically gets pushed and installed to it. Else you can fork the click packaged from the directory 'build'
