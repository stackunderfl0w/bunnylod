//
// Created by Pat on 4/4/2020.
//

#ifndef INC_3DGRAPHER_SHADERS_H
#define INC_3DGRAPHER_SHADERS_H


const char* simplevertextransform =
        "#version 100\n"
        "	// Input vertex data, different for all executions of this shader.	\n"
        "	attribute vec3 vertexPosition_modelspace;							\n"
        "	// Values that stay constant for the whole mesh.					\n"
        "	uniform mat4 MVP;													\n"
        "	void main(){														\n"
        "  // Output position of the vertex, in clip space : MVP * position		\n"
        "  		gl_Position =  MVP * vec4(vertexPosition_modelspace,1);			\n"
        "	}																	\n"
        "\n";

const char* singlecolorfragment =
        "#version 100                                                           \n"
        "precision mediump float;                                               \n"
        "   							                                        	\n"
        "    void main(){				                                                	\n"
        "      gl_FragColor = vec4(1,1,1,1);		                            \n"
        "      //gl_FragColor = vec4(vec3(gl_FragCoord.z/10.0),1.0);			\n"

        "    }							                                    	\n";

const char* basiclightingvertextransform =
        "#version 100\n"
        "attribute vec3 aPos;										\n"
        "attribute vec3 aNormal;									\n"
        "															\n"
        "varying vec3 FragPos;											\n"
        "varying vec3 Normal;											\n"
        "															\n"
        "uniform mat4 model;										\n"
        "uniform mat4 view;											\n"
        "uniform mat4 projection;									\n"

        "void main()												\n"
        "{															\n"
        "    FragPos = vec3(model * vec4(aPos, 1.0));				\n"
        "    //Normal = mat3(transpose(inverse(model))) * aNormal;	\n"
        "    Normal=aNormal;										\n"
        "															\n"
        "    gl_Position = projection * view * vec4(FragPos, 1.0);	\n"
        "}															\n";

const char* basiclightingfragment =
        "#version 100\n"
        "precision highp  float;											\n"
        "varying vec3 Normal;													\n"
        "varying vec3 FragPos;													\n"

        "uniform vec3 lightPos;												\n"
        "uniform vec3 viewPos;												\n"
        "uniform vec3 lightColor;											\n"
        "uniform vec3 objectColor;											\n"

        "void main()														\n"
        "{																	\n"
        "    // ambient														\n"
        "    float ambientStrength = 0.1;									\n"
        "    vec3 ambient = ambientStrength * lightColor;					\n"
        "																	\n"
        "    // diffuse														\n"
        "    vec3 norm = normalize(Normal);									\n"
        "    vec3 lightDir = normalize(lightPos - FragPos);					\n"
        "    float diff = max(dot(norm, lightDir), 0.0);					\n"
        "    vec3 diffuse = diff * lightColor;								\n"
        "																	\n"
        "    // specular													\n"
        "    float specularStrength = 0.5;									\n"
        "    vec3 viewDir = normalize(viewPos - FragPos);					\n"
        "    vec3 reflectDir = reflect(-lightDir, norm);					\n"
        "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);	\n"
        "    vec3 specular = specularStrength * spec * lightColor;			\n"
        "																	\n"
        "    vec3 result = (ambient + diffuse + specular) * objectColor;	\n"
        "    gl_FragColor = vec4(result.rgb, 1.0);								\n"
        "}																	\n"
        " 																	\n";
const char* vertex2d0 = R"glsl(

#version 100
attribute vec3 aPos;
attribute vec3 aTexCoord;
uniform float ratio;

varying vec2 TexCoord;

void main()
{
    //gl_Position = projection*vec4(aPos.x/ratio,aPos.y,aPos.z, 1.0);
    gl_Position = vec4(aPos.x/ratio,aPos.y,aPos.z, 1.0);


    //gl_Position = projection * view * vec4(vec3(model * vec4(aPos, 1.0)), 1.0);

    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
    )glsl";
const char* fragment2d0 = R"glsl(

#version 100
precision highp  float;

varying vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
	gl_FragColor = texture2D(texture1, TexCoord);
}
    )glsl";

const char *simpletexturevertex = R"glsl(
#version 100

// Input vertex data, different for all executions of this shader.
attribute vec3 vertexPosition_modelspace;
attribute vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}
    )glsl";
const char *simpletexturefragment = R"glsl(
#version 100
precision lowp  float;

// Interpolated values from the vertex shaders
varying vec2 UV;

// Ouput data
//out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){

	// Output color = color of the texture at the specified UV
	gl_FragColor = texture2D( myTextureSampler, UV );
    if(gl_FragColor.a==0.0){
    discard;
    }

}    )glsl";
const char *modelvs = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

}
    )glsl";
const char *modelfs = R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
}
    )glsl";

const char *pickingvs = R"glsl(

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

}
    )glsl";
const char *pickingfs = R"glsl(


#version 330 core

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform vec4 PickingColor;

void main(){

	color = PickingColor;

}
    )glsl";
const char *colorsvs = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
    )glsl";
const char *colorsfs = R"glsl(
#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform int point_lights_count = 4;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: point lights
    for(int i = 0; i < point_lights_count; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // phase 3: spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
    )glsl";
const char *lampvs = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
    )glsl";
const char *lampfs = R"glsl(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0); // set alle 4 vector values to 1.0
}
    )glsl";

const char* vertex2d = R"glsl(

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 vertexUV;

out vec2 UV;

void main()
{
    gl_Position = vec4(aPos, 1.0);
	UV = vertexUV;
}
    )glsl";
const char* singlecolorfragment2d = R"glsl(

#version 330 core
in vec2 UV;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D Diffuse;

void main(){

    // Output color = color of the texture at the specified UV
    color = vec3(1,1,1);
}
    )glsl";
const char* fragment2d = R"glsl(

#version 330 core
in vec2 UV;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D Diffuse;

void main(){

    // Output color = color of the texture at the specified UV
    color = texture( Diffuse, UV ).rgb;
}
    )glsl";

#endif //INC_3DGRAPHER_SHADERS_H
