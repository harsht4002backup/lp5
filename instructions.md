1. Download msys https://www.msys2.org/ -> msys2-x86_64-20260322.exe
2. Run the installer.
3. Run Msys lul 
4. Run `pacman -Syu`
5. Say Y to everything nigga
6. Isko run kr na bacchi `pacman -S mingw-w64-ucrt-x86_64-gcc`
7. Window ko hata na bhidu
8. UCRT cha path uchal na bhai `C:\msys64\ucrt64\bin`
9. Right click on This PC
10. Go to properties
11. Click on Advanced system settings
12. Click on environment variables under 'Advanced Tab'
13. Under system variables edit 'Path' and add the copied path `C:\msys64\ucrt64\bin`
14. Test in cmd whether g++ is installed or not `g++ -fopenmp`
15. Execute this in vscode terminal (first open the git cloned folder in vscode)
	`g++ -fopenmp .\hpc\hpc_prac1.cpp -o hpc_pract1`
16. Execute your program .\hpc_pract1.exe
