#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_ARB_separate_shader_objects : enable

//#define CLIPPINGREGION
//#define PREMULTIPLY_ALPHA

#include "../Base/Fragment.glsl"

Material material;
vec4 baseColor;
int textureID;

void main()
{
#ifdef CLIPPINGREGION

#endif
    
   
    if (materialID != -1)
    {
        material = materials[materialID];
		 
		uvec2 screencoord;
		screencoord.x = uint(gl_FragCoord.x);
		screencoord.y = uint(gl_FragCoord.y);
		if (screencoord.x < material.diffuseColor.x || screencoord.y < material.diffuseColor.y || screencoord.x > material.diffuseColor.z || screencoord.y > material.diffuseColor.w){ 
			discard;  
		}
		else
		{
			baseColor = color;
			textureID = GetMaterialTextureHandle(material, TEXTURE_DIFFUSE);
			if (textureID != -1) baseColor *= texture(texture2DSampler[textureID], texcoords.xy, TEXTURE_DIFFUSE);
		}
    }
    outColor[0] = baseColor;
	outColor[0].rgb *= outColor[0].a;
}