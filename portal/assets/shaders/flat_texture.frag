#version 330 core

uniform sampler2D textureSampler;
uniform vec4 PropId;

in vec2 uv;

float lerp(float alpha, float a, float b)
{
  return (b - a) * alpha + a;
}

void main()
{
  float dist = (gl_FragCoord.w / gl_FragCoord.z);
  float atenuation = lerp(dist, 0.3, 0.8);

  gl_FragColor = texture(textureSampler, uv);
  gl_FragColor *= min(atenuation, 1.0);
  //gl_FragColor = PropId;
}
