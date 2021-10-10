#version 330 core
layout(location = 0) in vec4 aPos;

out vec2 our_Color;
out vec2 tex_Coord;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);

    our_Color    = vec2(aPos.x, aPos.y);
    tex_Coord    = vec2(aPos.z, aPos.w);
}