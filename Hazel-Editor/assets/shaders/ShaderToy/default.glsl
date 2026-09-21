// ShaderToy-style shader.
// Only mainImage() (and helper functions) are needed - the engine injects the
// vertex shader, the uniform protocol and the main() wrapper.
//
// Provided by the adapter:
//   iResolution (vec3), iTime (float), iTimeDelta (float), iFrame (int),
//   iMouse (vec4), iDate (vec4)
// iChannel0..3 samplers exist but are not bound to textures yet.

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	vec2 uv = (2.0 * fragCoord - iResolution.xy) / iResolution.y;

	float r = length(uv);
	float a = atan(uv.y, uv.x);

	vec3 col = 0.5 + 0.5 * cos(iTime + vec3(0.0, 2.0, 4.0) + r * 6.0 - a * 2.0);
	col *= smoothstep(1.4, 0.2, r);

	fragColor = vec4(col, 1.0);
}
