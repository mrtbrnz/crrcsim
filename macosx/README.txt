Please find below what to do to be able to build this application with xcode.


1.Developpment folder
First of all, create an empty folder anywhere you would like ex:developpment.


2.Required libraries :  portaudio, plib, sdl
Compile portaudio, plib, sdl libraries. If you would like to avoid this compile process, please request them on the mailing list. Archive file contains precompiled object files ready to be used for next step. If you get more fresh compile version available, let's inform mailing list.
You should get object files in developpment/portaudio, developpment/plib, developpment/SDL.framework subfolders.

3.Get crrcsim
Checkout crrcsim from last cvs or download last version source in download area of sourceforge.

4.Available folders
At this step, you should get the following folders
developpment/portaudio
developpment/plib
developpment/SDL.framework
developpment/crrcsim
developpment/crrcsim/macosx/xcode

You are now ready to open xcode file : developpment/crrcsim/macosx/xcode/crrcsim.xcode

If you would like to activate cvs repository, do not forget to activate it in crrcsim project information. Choose cvs as SCM system (SCM system field) and unable SCM sytem.

5.Two build targets
There is 2 build targets available : crrcsim and crrcsim.dmg.tgz
a.crrcsim target build a standalone application with all required libraries embended in the application. No need to download separatly required libraries.
b.crrcsim.dmg.tgz target build a disk image containing application. crrcsim.dmg.tgz generated file will be directly used for release after beta testing.

