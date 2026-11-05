# Wikirace Ruiner

This is a basic program which searches through a [2013 dataset](https://snap.stanford.edu/data/enwiki-2013.html) of Wikipedia hotlinks to effectively automate the Wikirace game. The point of the game is that you start with a single wikipedia page and must find another through only hotlinks starting from the first page.

# Building

So far, build instructions are only for Windows and require Microsoft Visual Studio (MSVC) 2022 to be installed and properly configured to build C/C++ executables. This program also requires Qt, but the build instructions will help with installing the software using 'aqt' instead of needing to go through Qt's official installer and needing to make an account.

### Required Files:

Download both the [names database file](https://drive.google.com/file/d/1WBsT2R-SK2_qhtLDH1bswcXLEjqdtKr7/view?usp=sharing) and [small dataset](https://drive.google.com/file/d/1-vgvqlyQezky3QhdUHd_NSF6iviLEkuF/view?usp=sharing). Once installed, place them into the `database` and `dataset` folders respectively. **The program requires both files to be in their correct location or else it will not work.** We recommend using the smaller dataset because the larger one has over **100,000,000** edges, which is not great for memory and slows the program down quite drastically. Given the scope of the this project, it is better to just use the smaller dataset.

### Installing Qt

Go inside the 'gui' folder and run `installQt.bat`. It will install the MSCV 2022_64 version Qt 6.10.3 into `C:/Qt`. This installation is required for the rest of the program to build.

### Building the executable

Once Qt is installed, run `buildApp.bat`. This script will compile and build the app along with deploying it via `windeployqt.exe` to make the app portable.



### Unnecessary Extra Files

[Large Dataset/Graph file](https://drive.google.com/file/d/1cvz6xFD6SEfjfP8NitmNrqgDJa8FiXvT/view?usp=sharing)
