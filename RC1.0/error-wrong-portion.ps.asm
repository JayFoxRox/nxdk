!!RC1.0

{
  rgb {
    spare0 = spare0.b;
    spare1 = spare1.b;
  }
  alpha {
    spare0 = spare0.rgb;
    spare1 = spare1.rgb;
  }
}

out.rgb = spare0.b;
out.a = spare0.rgb;
