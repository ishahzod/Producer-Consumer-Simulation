Instructions for TA ZongXing Xie,

       I'm pretty sure you already know all this since you're a pro. But here it is nonetheless:

1) Extract the files from the zip file using 7zip or Winrar.
2)Open project2.cpp file using any editor(notepad, notepad++, atmel, visual studio) etc.
3) You can read my comments which followed the template that was posted on BB.
4) Select all (ctrl + a) and copy (ctrl + c) the source code. Paste the code in gedit on linux. 
5) Compile on a Linux machine, or using PuTTy g++ test.cpp -o test
6) Create two txt files in whatever directory you wish.
7) Acquire the path of that file and the file you want its contents copied to
	 using readlink -f textfile.txt
8) Run code ./test
9) Enter the source path and the destination path accordingly
10) Press Enter. You should see that the contents were copied.
11) if you cat source and cat destination files, they should be the same now. 

Thank you!
-Shahzod

Have a nice day!
Thanks, 
Shahzod. 
