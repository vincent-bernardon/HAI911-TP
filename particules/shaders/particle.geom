#version 430

#extension GL_EXT_geometry_shader4 : enable
#extension GL_EXT_gpu_shader4 : enable

layout ( points ) in;

// Shader de passage : passe de point vers point
// layout ( points ) out;
// Shader utile : passe de point vers triangles
layout ( triangle_strip, max_vertices = 4 ) out;

// Les matrices peuvent être réutilisées
uniform mat4 mvp;

// Variables d'entrées : des tableaux
in vec3 initialVertPos[];

in vec4 Color[]; //pour faire changer la couleur 
out vec4 Colorimg; //pour faire changer la couleur 

// Variables de sorties : des éléments uniques
out vec2 UV;

//Geometry Shader entry point
void main(void) {
	// Taille d'une particule
	float scale = 0.05;

	
	// gl_Position = gl_in[0].gl_Position;
	// EmitVertex();

	// Position centrale de la particule
    vec4 center = gl_in[0].gl_Position;

    // Définir les 4 sommets du quad
    vec4 topLeft = center + vec4(-scale, scale, 0.0, 0.0);
    vec4 topRight = center + vec4(scale, scale, 0.0, 0.0);
    vec4 bottomLeft = center + vec4(-scale, -scale, 0.0, 0.0);
    vec4 bottomRight = center + vec4(scale, -scale, 0.0, 0.0);

    gl_Position = topLeft;
	UV = vec2(0.0, 1.0);
    Colorimg=Color[0];
    EmitVertex();

    gl_Position = bottomLeft;
	UV = vec2(0.0, 0.0);
    Colorimg=Color[0];

    EmitVertex();

    gl_Position = topRight;
	UV = vec2(1.0, 1.0);
    Colorimg=Color[0];

    EmitVertex();

    gl_Position = bottomRight;
	UV = vec2(1.0, 0.0);
    Colorimg=Color[0];

    EmitVertex();
	
	EndPrimitive();
}