## RayTracer

This ray tracer renders a randomized scene (plane + spheres + point lights) and writes an output PNG (default: `test.png`) to the project root.

## Usage

Run with defaults:

```bash
raytracer.exe
```

Print all options:

```bash
raytracer.exe --help
```

Example (move camera and increase lighting):

```bash
raytracer.exe --cam-pos -18 12 18 --cam-fov 55 --num-lights 6 --light-intensity 0.2 1.0
```

## Command line arguments

All arguments are optional; if omitted, the default shown below is used.

### Output

- `--out <filename>`: Output image filename. Default: `test.png`

### Camera

- `--cam-pos <x> <y> <z>`: Camera position. Default: `-12 10 12`
- `--cam-fov <degrees>`: Camera vertical field-of-view (degrees). Default: `45`

### Lights (randomized)

- `--num-lights <n>`: Number of point lights. Default: `3`
- `--light-pos-x <min> <max>`: Light X position range. Default: `-12 12`
- `--light-pos-y <min> <max>`: Light Y position range. Default: `10 14`
- `--light-pos-z <min> <max>`: Light Z position range. Default: `-12 12`
- `--light-intensity <min> <max>`: Light intensity range. Default: `0.1 0.8`

### Plane (randomized)

- `--plane-color <min> <max>`: Plane diffuse RGB component range (the same range is used for R/G/B). Default: `0.1 1.0`

### Spheres (randomized)

- `--num-spheres <n>`: Total number of spheres. Default: `75`
- `--sphere-pos-xz <min> <max>`: Sphere X/Z position range. Default: `-10 10`
- `--sphere-y <min> <max>`: Sphere Y position range. Default: `0.25 2.0`
- `--sphere-radius <min> <max>`: Sphere radius range. Default: `0.25 0.75`
- `--sphere-color <min> <max>`: Sphere diffuse RGB component range. Default: `0.1 1.0`
- `--sphere-spec <min> <max>`: Sphere specular RGB component range (same value used for R/G/B). Default: `0.1 0.5`
- `--sphere-ambient-scale <min> <max>`: Ambient term scale, where `ambient = scale * diffuse`. Default: `0.03 0.12`
- `--sphere-exponent <min> <max>`: Phong exponent range. Default: `15 200`

### Help

- `-h`, `--help`: Print usage and exit.
