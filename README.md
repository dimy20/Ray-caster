# Ray-caster

A Wolfenstein 3D-style raycasting engine built from scratch in C++17 using SDL2.

![Demo](early_demo.gif)

## About

This engine renders a pseudo-3D first person view from a 2D grid map. 
It casts one ray per screen column using DDA-style grid traversal to find wall intersections, then draws textured wall slices scaled by distance. 
Floors and ceilings are rendered via inverse screen-to-world mapping, and sprites are projected into screen space with depth sorting and occlusion against walls. 
The common Fish-eye distortion issue is corrected using perpendicular distance projection.

## Features

- Textured walls, floors, and ceilings
- Sprite rendering with transparency and depth testing
- 800x600 projection plane upscaled to 1280x768


- This project implemented the 'core' of a raycasting engine.

Additional features could be:
  - Doors and interactive elements
  - Dynamic lighting and shading by distance
  - Weapon rendering and combat

## Controls

| Key | Action |
|-----|--------|
| W / S | Move forward / backward |
| A / D | Rotate left / right |

## Building

Requires **SDL2** and **SDL2_image**.

```bash
make
./main
```
