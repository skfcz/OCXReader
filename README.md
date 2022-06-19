# OCXReader #

This should become a reader for OCX file into OpenCascade.
The general aim is to create STEP and GLTF files for visualization.

### What to expect ###
In the long run product structure like 

````shell
${vesselname}
|__ Reference Planes
|  |__ XRefPlanes
|  |  |__ Frame 1 ... Frame N
|  |__ YRefPlanes
|  |  |__ Longitudinal 1 ... Longitudinal N
|  |__ ZRefPlanes
|     |__ Deck 1 ... Deck N
|__ Reference Surfaces
|  |__ Ref Surface 1 ... Ref Surfaces N
|     |__ Contour
|     |__ Face   
|__ Panels
   |__ Panel 1 ... Panel N
      |__ Contour
      |__ Face
      |__ ComposeOf
         |__ Plate 1 ... Plate N
         |__ Stiffener 1 ... Stiffener N  
````

Currently, I'm working on getting all curves (contours, stiffener traces) right.

### Disclaimer ###
My day to day programming language is Java and not C++ :-)

### How do I get set up? ###
I develop the software on my Linux Box, compiling and running on Windows is
not tested yet.

* Run cmake
* Run make/ninja/vs .. to compile

### Who do I talk to? ###

Carsten Zerbst (carsten.zerbst@groy-groy.de)

### License ###
[GNU LESSER GENERAL PUBLIC LICENSE v. 2.1](LICENSE)