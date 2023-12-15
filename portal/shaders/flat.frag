#version 110

varying vec3 color;

void main()
{
  float dist = (gl_FragCoord.z / gl_FragCoord.w);
  float desaturation = (1.0 / dist) * 5.0;
  gl_FragColor = vec4(color, 1.0) * desaturation;
}