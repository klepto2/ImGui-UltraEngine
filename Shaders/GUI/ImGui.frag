#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_draw_parameters : enable
//#extension GL_EXT_multiview : enable
#extension GL_ARB_bindless_texture : enable


#include "../Base/Fragment.glsl"


void main()
{
	vec4 baseColor = color;
    if (materialID != -1)
    {
        Material material = materials[materialID];
		 
		uvec2 screencoord;
		screencoord.x = uint(gl_FragCoord.x);
		screencoord.y = DrawViewport.w - 1 - uint(gl_FragCoord.y);
		if (screencoord.x < material.diffuseColor.x || screencoord.y < material.diffuseColor.y || screencoord.x > material.diffuseColor.z || screencoord.y > material.diffuseColor.w){ 
			discard;  
		}
		else
		{
			baseColor = color;
			if (material.textureHandle[TEXTURE_DIFFUSE] != uvec2(0))
				baseColor *= textureLod(sampler2D(material.textureHandle[TEXTURE_DIFFUSE]), texcoords.xy, 0);
		}
    }
	
    outColor[0] = baseColor;
	outColor[0].rgb *= outColor[0].a;
}
