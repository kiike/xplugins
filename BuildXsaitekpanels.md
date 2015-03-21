# Introduction #

This page provides instructions how to build Xsaitekpanels on Linux, Windows or a Mac.

The first thing I would do is to use this thread an make sure you can build a hello world.

[How to setup your universal X-Plane plugin dev environment on every platform](http://forums.x-plane.org/index.php?showtopic=48012)

I have my Linux development computer set up in the following manner.

On my home I have the folder

**xplugins**

In the xplugins folder I have these three folders

**SDK**

**hidapi-0.7.0**

**Xsaitekpanels**

The source code from this site go in the Xsaitekpanels folder and using the following commands will do this for you. Change bill to your computer home name.

cd /home/bill/xplugins

svn checkout http://xplugins.googlecode.com/svn/trunk/Xsaitekpanels Xsaitekpanels

Now, there are some prerequisities to download...


  * **X Plane SDK 2.0.1** - http://www.xsquawkbox.net/xpsdk/XPSDK201.zip

  * **Signal11 / Hidapi** - https://github.com/signal11/hidapi/downloads

> Click on the Download as Zip button for the latest version.

# **Building Xsaitekpanels on Linux** #

This is my Build settings for Ubuntu 64 Bit

![http://xplugins.googlecode.com/svn/wiki/img/LinBuildSettings.png](http://xplugins.googlecode.com/svn/wiki/img/LinBuildSettings.png)

This is my Run Settings

![http://xplugins.googlecode.com/svn/wiki/img/LinRunSettings.png](http://xplugins.googlecode.com/svn/wiki/img/LinRunSettings.png)

# **Building Xsaitekpanels on Windows** #

This is my Build settings for Windows 7 64 Bit

http://xplugins.googlecode.com/svn/wiki/img/Win7BuildSettings.PNG

This is my Run Settings

http://xplugins.googlecode.com/svn/wiki/img/Win7RunSettings.PNG

# **Building Xsaitekpanels on a Mac** #

This is my Build settings for a Mac

![http://xplugins.googlecode.com/svn/wiki/img/MacBuildSettings.png](http://xplugins.googlecode.com/svn/wiki/img/MacBuildSettings.png)

This is my Run Settings

![http://xplugins.googlecode.com/svn/wiki/img/MacRunSettings.png](http://xplugins.googlecode.com/svn/wiki/img/MacRunSettings.png)