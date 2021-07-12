# Dust particle generator

A Unix program that creates mcfunction files that spawn dust particle clouds, from the vertices described in an OBJ file, and one that creates said files from a hard-coded bounding box.

Use MinGW or the WSL for usage on Windows. If you want compilation using the MSVC to be available, react to [this issue](https://github.com/Alluysl/minecraft-dust-cloud-generator/issues/3).

## Compilation

Run `make` or `make gen_obj_color` to compile the OBJ-based generator that uses a texture.

Run `make gen_obj` to compile the OBJ-based generator that uses a plain color.

Run `make gen_grid` to compile the bounding-box-based generator.

Run `make gen_obj_example` for an example that uses `shape.obj` (needs to be provided) to create a `red_shadow` and a `cyan_shadow` functions.

Run `make clean` to remove the executables.

## Usage

`gen_obj_color <input file> <output file> <RGB-driving image file> <particle size> <boxX> <boxY> <boxZ> <speed> <count> <chance of the particle being 'force' instead of 'normal'>` (the alpha channel multiplies the scale of the particles and their 'force' chance)

`gen_obj <input file> <output file> <r> <g> <b> <particle size> <boxX> <boxY> <boxZ> <speed> <count> <chance of the particle being 'force' instead of 'normal'>`

`gen_grid` (hard-coded values, change them in `gen_grid.c`)

## Limitations

### Generators

* Both OBJ-based generators only consider vertices, they don't "fill" faces with particles.
* The texture-based generator can't find out which pixel of the texture to sample for a vertex which's UV lands right on the boundary between two pixels, and will always sample the one at the bottom right (except if the UV is at the bottom/right edge of the image, then it selects the last pixel of the column/row) no matter where the other vertices of the face are. [I plan to fix this](https://github.com/Alluysl/minecraft-dust-cloud-generator/issues), in the meantime a workaround is to make it so that UVs of different faces don't share pixels, and double the bottom and right pixels at the borders of UVs.
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
