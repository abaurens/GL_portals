#version 110

varying vec3 color;

float lerp(float alpha, float a, float b)
{
  return (b - a) * alpha + a;
}

void main()
{
  float dist = (gl_FragCoord.w / gl_FragCoord.z);
  float atenuation = lerp(dist, 0.3, 0.8);

  gl_FragColor = vec4(color, 1.0);
  gl_FragColor *= atenuation;
}