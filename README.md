# LEVR (Low Effort Voxel R(ay*er)enderer)
## GIF demonstration
![](https://github.com/Ilya3point999K/LEVR/blob/main/levr_show.gif)
## What's that?
Since nobody on the Internet has bothered to release a SIMPLE demonstration of HOW TO render voxels without bells and whistles (Compute Shaders, Vulkan, CUDA, whatever other blasphemous technology no one uses) - I had to do it.
## Ok, what it has?
* <1000 LOC, C99, single header + demo program
* Voxel world represented with flat 3D array of material indecies
* Casts rays in arbitrary resolution
* Software rendering, no GPU
* Per-Voxel Point lights
* Per-Voxel Shadows
* Optional parallelism with OpenMP
* Optional checkerboard rendering
## How to build
`make`, that is all. Demo uses `app.h` by [Mattias Gustavsson](https://github.com/mattiasgustavsson/libs/tree/main), very cool one, but you can easily replace it with whatever framebuffer implementation you desire.
## TO DO
* Make light and shadows more correct and less ugly
* Transfer to fixed point instead of floats
* Replace rotation function with something more correct roll-wise
* Actual recursive tracing with mirrors and other things
* Shrink down voxels
* Further optimisations
## Contributing
You can help improve LEVR in a way I'll approve, otherwise, you could fork it and do whatever. Pull requesting here implies you're ok with releasing your contribution to public domain.
## License
CC0 1.0 Universal, Public Domain
