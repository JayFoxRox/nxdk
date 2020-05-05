mesh
====

![screenshot](screenshot.png)

Example of drawing real geometry with textures and lighting.

## Generated files

This sample uses generated source-code files.

### verts.h

Contains the vertices and indices which describe the triangles for the mesh.

See verts.py for more information.

In a real-world application you'd fill the `vertices` and `indices` arrays at runtime.
Typically by loading them from a file that is suitable for your use-case.

### texture.h

Contains the texture description and pixel data.

See texture.py for more information.

In a real-world application you'd fill the pixel array at runtime.
Typically by loading it from a file that is suitable for your use-case.

## Notes

### UV coordinates

The Xbox GPU differentiates between swizzled, linear textures and compressed texture formats.

- Linear texture formats:
  - Data is stored line-by-line.
  - Various pixel formats.
  - Bad performance.
  - Can use arbitrary width and height.
  - Use absolute texture coordinates.
- Swizzled texture formats:
  - Data is swizzled (re-arranged) into pixel blocks.
  - Various pixel formats.
  - Good performance (cache-friendly).
  - Must have width and height which are power-of-two.
  - Use normalized texture coordinates.
- Compressed texture formats:
  - Data must be compressed as S3TC / DXT.
  - Compression might cause artefacts.
  - Good performance (cache-friendly).
  - Must have width and height which are power-of-two.
  - Use normalized texture coordinates.

This sample uses a linear-texture format.
Therefore, the UV coordinates in this sample are non-normalized.

In a real-world application you'd swizzle or compress your texture.
Accordingly, your meshes would use normalized texture-coordinates.
