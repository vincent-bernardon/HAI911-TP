// --------------------------------------------------
// shader definition
// --------------------------------------------------

#version 130

// --------------------------------------------------
// Uniform variables:
// --------------------------------------------------
	uniform float xCutPosition;
	uniform float yCutPosition;
	uniform float zCutPosition;

	uniform float xMax;
	uniform float yMax;
	uniform float zMax;

	uniform mat4 mv_matrix;
	uniform mat4 proj_matrix;

// --------------------------------------------------
// varying variables
// --------------------------------------------------
varying vec3 position;
varying vec3 textCoord;
// --------------------------------------------------
// Vertex-Shader
// --------------------------------------------------


void main()
{
	gl_Position = proj_matrix * mv_matrix * gl_Vertex;
	position = gl_Vertex.xyz;

	//Todo : compute textCoord

}
