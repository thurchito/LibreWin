i686-w64-mingw32-gcc -nostdlib -o open.exe applications/program.c
i686-w64-mingw32-gcc -nostdlib -o hello.exe applications/hello.c applications/appinclude/print.c -Iapplications/appinclude
i686-w64-mingw32-gcc -nostdlib -o blorp.exe applications/blorp.c applications/appinclude/print.c -Iapplications/appinclude
i686-w64-mingw32-gcc -nostdlib -o freever.exe applications/buildno.c
i686-w64-mingw32-gcc -nostdlib -o stop.exe applications/shutdown.c
i686-w64-mingw32-gcc -nostdlib -o reboot.exe applications/reboot.c
i686-w64-mingw32-gcc -nostdlib -o bsod.exe applications/bsod.c
