## Pulling your UrlLib branch from CMake

Since the polyfills in this repo depend on UrlLib, but are split into
different repos (JsRuntimeHost and UrlLib), we are using CMake at build time to pull 
UrlLib into the local JsRuntimeHost build. In the root directory where this README.md is 
there is also a CMakeLists.txt file. Inside of this CMake file there are two 
**FetchContent_Declare()** commands declaring *arcana* and *UrlLib*. If you are testing 
changes from UrlLib then you need to change the **GIT_REPOSITORY** variable to the 
UrlLib repo/fork you are pulling from and **GIT_TAG** variable to the branch you are 
testing starting with origin/{BranchName}. Ex. if your branch on UrlLib is called "Test" 
your **GIT_TAG** would be origin/Test.
