/* Construct a viewport transformation matrix */
static void matrix_viewport(float out[4][4], float x, float y, float width, float height, float z_min, float z_max)
{
    float m[4*4] = {
      width/2.0f, 0.0f,         0.0f,          x + width/2.0f,
      0.0f,       height/-2.0f, 0.0f,          y + height/2.0f,
      0.0f,       0.0f,         z_max - z_min, z_min,
      0.0f,       0.0f,         0.0f,          1.0f
    };
    memcpy(out, m, sizeof(m));
}

