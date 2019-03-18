#version 440 core

#define COLOR 1

in vec2 texCoord;
out vec4 color;

uniform int step;
uniform float h_max;
uniform sampler2D ourTexture;

float interpolate( float val, float y0, float x0, float y1, float x1 ) {
    return (val-x0)*(y1-y0)/(x1-x0) + y0;
}

float base( float val ) {
    if ( val <= -0.75 ) return 0;
    else if ( val <= -0.25 ) return interpolate( val, 0.0, -0.75, 1.0, -0.25 );
    else if ( val <= 0.25 ) return 1.0;
    else if ( val <= 0.75 ) return interpolate( val, 1.0, 0.25, 0.0, 0.75 );
    else return 0.0;
}

float red( float gray ) {
    return base( gray - 0.5 );
}
float green( float gray ) {
    return base( gray );
}
float blue( float gray ) {
    return base( gray + 0.5 );
}

void main()
{
	float h = texture(ourTexture, texCoord)[step] / h_max;
#if COLOR == 1
	color = vec4(red(h), green(h), blue(h), 0.0f);
#else
	if (h > 1.0)
	    color = vec4(1, 0, 0, 0);
	else if (h < -1.0)
	    color = vec4(0, 0, 1, 0);
	else
	    color = vec4(vec3(1.0f) * (h + 1) * 0.5f, 0.0f);
#endif
}