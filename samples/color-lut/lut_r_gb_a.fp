!!TS1.0
texture_2d(); // RGBA0[a8r8g8b8] = tex0
dependent_ar(tex0); // tex0.r  -> Rgba1[y8] = Look up new R value from LUT 1x256 LUT
dependent_gb(tex0); // tex0.gb -> rGBa2[g8b8] = Look up new G and B value from LUT 256x256 LUT
dependent_ar(tex0); // tex0.a  -> rgbA3[y8] = Look up new A value from LUT 256x1 LUT

!!RC1.0
{
  const0 = (1.0, 0.0, 0.0, 0.0);
  rgb {
    discard = tex1.rgb * const0.rgb; // ab = R
    discard = tex2.rgb; // cd = GB
    spare0 = sum(); // abcd = R+GB
  }
}
out.rgb = spare0.rgb;
out.a = tex3.b;
