#version 430

out vec4 fragColor;

// Input du geometry shader
in vec2 UV;

in vec4 Colorimg; //pour fiare changerles couleur des particules

// uniform vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

uniform sampler2D particleTexture;

void main(void)
{
    // fragColor = color;
    
    // Utiliser les coordonnées UV pour débugger : un cadre noir autour des particules
    // if (UV.x < 0.1 || UV.x > 0.9 || UV.y < 0.1 || UV.y > 0.9)
    //     fragColor = vec4(0.0, 0.0, 0.0, 1.0);

    // Utiliser une texture pour les particules
    vec4 texColor = texture(particleTexture, UV);
    if(texColor.a < 0.1)
        discard; // Ne pas afficher les pixels transparents
    fragColor = texColor * Colorimg;
    // fragColor = Colorimg;
}