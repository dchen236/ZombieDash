# ZombieDash
Zombie Dash 
Zombie Dash is an project designed by Professor Carey Nachenberg http://careynachenberg.weebly.com/cs-slides.html
![screen shot 2019-02-23 at 19 38 22](https://user-images.githubusercontent.com/32973628/53294791-983ee880-37a2-11e9-8bfb-1f8cb1f7c577.png)

# Instruction to run the program on macOs:

macOS: You'll need to have XQuartz and freeGLUT installed to run the program.

Install XQuartz:
Click the download link, open the downloaded .dmg file, double-click on XQuartz.pkg, and follow the installation instructions.
Log out and log back in again.
To verify the installation, open a Terminal window and run the command echo $DISPLAY. That should produce one line of output that ends with org.macosforge.xquartz:0.

Install freeGLUT:
Install the homebrew package manager.
Open a Terminal window and run the command brew install freeglut.

Suggestion: It would be much more convinient if you have Xcode installed. 
However if your running it in terminal make sure the Assets is in the same folder as the executable program

Troubles some of the user may encounter when using Xcode
You might get a message at startup relating to the Assets directory or level data not loaded properly etc. 
If instead you want to fix it for every project you create with Xcode, select Xcode / Preferences/Location and for the Derived Data, select Relative.
If you just want to fix the issue just for this project, select File / Project Settings... and for Derived Data Location, select Project-relative Location. 


