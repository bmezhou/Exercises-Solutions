#version 330 core
out vec4 frag_Color;

in vec2 our_Color;
in vec2 tex_Coord;

uniform sampler2D texture1;

void main()
{
    frag_Color = texture(texture1, tex_Coord);
}