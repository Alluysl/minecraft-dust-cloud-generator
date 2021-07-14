# Dust particle cloud generator

A Unix program that creates mcfunction files that spawn dust particle clouds, from the vertices described in an OBJ file, and one that creates said files from a hard-coded bounding box.

Use MinGW or the WSL for usage on Windows. If you want compilation using the MSVC to be available, react to [this issue](https://github.com/Alluysl/minecraft-dust-cloud-generator/issues/3).

## Compilation

Commands are to be run in the `C` folder, where the makefile is.

Run `make` or `make gen_obj_color` to compile the OBJ-based generator that uses a texture.

Run `make gen_obj` to compile the OBJ-based generator that uses a plain color.

Run `make gen_grid` to compile the bounding-box-based generator.

Run `make gen_obj_example` for an example that uses `shape.obj` (needs to be provided) to create a `red_shadow` and a `cyan_shadow` functions.

Run `make clean` to remove the executables.

## Usage

`gen_obj_color <input file> <output file> <RGB-driving image file> <particle size> <boxX> <boxY> <boxZ> <speed> <count> <force chance> [<pixel precision>]`

`gen_obj <input file> <output file> <r> <g> <b> <particle size> <boxX> <boxY> <boxZ> <speed> <count> <force chance>`

`gen_grid` (hard-coded values, change them in `gen_grid.c`)

### Arguments rundown

* Input file (string): the path to an existing OBJ file containing the model to generate a function file from.
* Output file (string): the path to the output Minecraft function file (will create the file if it doesn't exist, and clear any old content of the file before writing).
* Image file: (string, `gen_obj_color` only) the path to texture to get RGBA values from when sampling UVs (essentially, the texture to apply to the model).
* R/G/B: (floats, `gen_obj` only): the color to give generated particles. Corresponds to the command argument.
* Particle size (float): corresponds to the command argument. Will be multiplied by the alpha value of the texture at the sampled pixel when using `gen_obj_color`.
* Box X/Y/Z (floats): the size of the bounding box around the anchor point where the particle will spawn at a random location; corresponds to the command argument. Preferable to leave at `0 0 0`.
* Speed (float): the speed (in a random direction) the particle will have; corresponds to the command argument. Preferable to leave at `0`.
* Count (integer): the amount of particles to spawn in a single command; corresponds to the command argument. Better left at `1`.
* Force chance (float): the chance, between 0 and 1, of any given particle command to be set as `force` instead of `normal`. Will be multiplied by the alpha value at the sampled pixel when using `gen_obj_color`. This will force the particle to display no matter the distance and particle settings. When set low, will make the cloud appear sparser at a distance. When set high, may cause lag on weak computers.
* Pixel precision *[optional]* (double-precision float): the UVs on the model may be placed on the border of a pixel, but not perfectly. In the advent of a UV wrongly bleeding into a pixel it shouldn't be in, setting this value above zero will - on each axis - select the closest pixel toward the inside of the face if close enough. Only use if needed and start with low values: try to find the lowest working value, as higher values will cause colors to become blurry as they become closer to the gap in pixels between vertices. Values above or close to 0.5 are expected to be broken, but shouldn't ever be needed, since that would mean the error corresponds to almost, if not more than half a pixel. Negative values have the same effect as zero, which is none.

## Limitations

### Generators

* Both OBJ-based generators only consider vertices, they don't "fill" faces with particles.
* That generator only supports albedo, for obvious reasons.
* Naturally, the single-color OBJ-based generator only outputs particles of one color.
* The arguments for the grid version are hard-coded. The grid isn't randomized, and is centered in the XZ plane, though this isn't hard to change.

### Minecraft

* **LAG** (reducing the proportion of force particles could mitigate it a little).
* Particle color will change slightly on a random basis.
* Particle shape/animation/direction will change slightly, possibly making the result look fuzzy.

## Gallery

### Texture

#### Steve

Model:

![Subdivided Steve model in Blender](img/steve_model.png)

Result (repeat):

![Result with a repeating command block](img/steve_repeat.png)

### Single color

#### Steve-like

Model:

![Model in Blender](img/steve_omni_model.png)

Result (impulse):

![Result with an impulse command block](img/steve_omni_impulse.png)

Result (repeat):

![Result with a repeating command block](img/steve_omni_repeat.png)

#### Crewmate

Model (by Ikki_3d, remeshed to reduce polycount):

![Crewmate model in Blender](img/crewmate_model.png)

Result (repeat):

![Result with a repeating command block](img/crewmate_repeat.png)

## Special thanks

This program makes use of the [`stb_image` library](https://github.com/nothings/stb/blob/master/stb_image.h).
