Essentia
========

Windows build for essentia with msvc (2019)

run MSYS2 under vs2019 command tool.

./waf configure --mode debug --msvc_targets=x64  --build-static --lightweight=fftw

./waf build -j4
