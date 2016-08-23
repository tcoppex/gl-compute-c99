# gl-compute-c99

## A Propos

A simple & self-contains Compute Shader sample using C99 and OpenGL 4.3.

A custom python script, _'glextgen'_, is provided to automically create at compilation-time the C binding code needed to load OpenGL extensions. 

## Quickstart

### Dependencies

This project keeps few dependencies :
- OpenGL 4.3+ [required]
- GLFW 3 [required]
- GLEW 1.10+ [optional]
- Python 2.7+ [if not using GLEW]

### Build

Enter your terminal in the project root directory and type the following :
```
$ mkdir build; cd build
$ cmake ../
$ cmake --build .
```

## Known Limitations

On Windows's MSVC, the thirdparties directory should be specified by hand on the CMakeLists.txt file.

## Acknowledgment

Noise and fbm shaders code from Iñigo Quilez.

## License

This project is released under *The Unlicense*.
