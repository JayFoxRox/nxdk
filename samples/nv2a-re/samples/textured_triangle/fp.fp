!!TS1.0

texture_2d();
nop();
nop();
nop();

!!RC1.0

{
  rgb {
    spare0.rgb = tex0.rgb * col0.rgb;
  }
  alpha {
    spare0.a = one;
  }
}

out.rgb = spare0.rgb;
out.a = spare0.a;
