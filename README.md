## Command to build
### In source folder enter the following commands into the terminal
```
$ cmake -B ./build
$ cmake --build ./build --target lsh -- -j 4
```
### Once done the `lsh` file with the program should be at `./build` directory.
#### In order to run it you can just type after the first two lines the following command
```
$ ./build/lsh
``` 