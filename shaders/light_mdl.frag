#version 330 core
#define MAX_DIR 2
#define MAX_POINT 16
#define MAX_SPOT 16

struct material {
    sampler2D   diffuse;
    sampler2D   specular;
	sampler2D   emission;
	samplerCube cubemap;
	sampler2D	reflectivity;
    float       shininess;
}; 

struct lightDirectional {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct lightPoint {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float linear;
	float quadratic;
	float constant;
};

struct lightSpot {
    vec3  position;
    vec3  direction;
    float innerCutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float linear;
	float quadratic;
	float constant;
};    
  
in vec3 v_normal;
in vec3 FragPos; 
in vec2 TexCoords; 

out vec4 color;
  
uniform vec3 viewPos;
uniform material mat;
uniform lightDirectional dir[MAX_DIR];
uniform lightPoint point[MAX_POINT];
uniform lightSpot spot[MAX_SPOT];
uniform vec3 light_sources; 

vec4 fog_color = vec4(0.8, 0.82, 0.83, 1.0);
float fog_density = 0.1f;

vec3 lightDirectional_Calc(lightDirectional lamp, vec3 viewDir, vec3 norm);
vec3 lightPoint_Calc(lightPoint lamp, vec3 viewDir, vec3 norm);
vec3 lightSpot_Calc(lightSpot lamp, vec3 viewDir, vec3 norm);
vec4 reflection_Calc();

void main()
{    
    vec3 norm = normalize(v_normal);
    vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result;
	// 1. Directional light
	for (int i = 0; i < light_sources.x; i++)
		result += lightDirectional_Calc(dir[i], viewDir, norm);
	// 2. Point light
	for (int i = 0; i < light_sources.y; i++)
		result += lightPoint_Calc(point[i], viewDir, norm);
	// 3. Spot light
	for (int i = 0; i < light_sources.z; i++)
		result += lightSpot_Calc(spot[i], viewDir, norm);
	// 4. Cubemap reflection
	vec4 ref = reflection_Calc();
    color = vec4(result, 1.0f) + ref;
	// 5. Fog
	/*float near = 0.1f, far = 100.0f;
	float z = gl_FragCoord.z * 2.0 - 1.0;
    float depth = (2.0 * near * far) / (far + near - z * (far - near));	
	
	float fog = clamp(exp(fog_density * (- depth)), 0, 1);
    vec4 fin_color = fog*color + (1.0 - fog) * fog_color;
	color = fin_color;*/
}

vec4 reflection_Calc()
{             
    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(v_normal));
	float ref = vec3(texture(mat.reflectivity, vec2(TexCoords.x, 1.0 - TexCoords.y))).r;
	ref *= sign(ref - 0.1);
	vec4 fin = texture(mat.cubemap, R) * ref;
    return fin;
    //return ref;
}

//***************** Light source calculation functions *****************//

vec3 lightDirectional_Calc(lightDirectional lamp, vec3 viewDir, vec3 norm){
	// Ambient
	vec3 ambient = lamp.ambient * vec3(texture(mat.diffuse, vec2(TexCoords.x, 1.0 - TexCoords.y)));
  	
    // Diffuse 
	vec3 lightDir = normalize(-lamp.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lamp.diffuse * diff * vec3(texture(mat.diffuse, vec2(TexCoords.x, 1.0 - TexCoords.y)));  
    
    // Specular
    vec3 halfVec = normalize(viewDir + lightDir);  
    float spec = pow(max(dot(norm, halfVec), 0.0), mat.shininess * 4.0);
    vec3 specular = lamp.specular * spec * vec3(texture(mat.specular, vec2(TexCoords.x, 1.0 - TexCoords.y)));  
	
	vec3 emission = vec3(texture(mat.emission, vec2(TexCoords.x, 1.0 - TexCoords.y)));
        
    return ambient + diffuse + specular + emission; 
}

vec3 lightPoint_Calc(lightPoint lamp, vec3 viewDir, vec3 norm){
	// Attenuation
	float lightConst = 1.0f;
	float dist = length(lamp.position - FragPos);
	float attenuation = 1.0f/(lightConst + lamp.linear*dist + lamp.quadratic *(dist*dist));
	vec3 attn = vec3(attenuation);

    // Ambient
	vec3 ambient = lamp.ambient * vec3(texture(mat.diffuse, vec2(TexCoords.x, 1.0 - TexCoords.y))) * attn;
  	
    // Diffuse 
    vec3 lightDir = normalize(lamp.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lamp.diffuse * diff * vec3(texture(mat.diffuse, vec2(TexCoords.x, 1.0 - TexCoords.y))) * attn;  
    
    // Specular
    vec3 halfVec = normalize(viewDir + lightDir);   
    float spec = pow(max(dot(norm, halfVec), 0.0), mat.shininess * 4.0);
    vec3 specular = lamp.specular * spec * vec3(texture(mat.specular, vec2(TexCoords.x, 1.0 - TexCoords.y))) * attn;  
	
	vec3 emission = vec3(texture(mat.emission, vec2(TexCoords.x, 1.0 - TexCoords.y)));
        
    return ambient + diffuse + specular + emission; 
}

vec3 lightSpot_Calc(lightSpot lamp, vec3 viewDir, vec3 norm){
	// Attenuation
	float lightConst = 1.0f;
	float dist = length(lamp.position - FragPos);
	float attenuation = 1.0f/(lightConst + lamp.linear*dist + lamp.quadratic *(dist*dist));
	vec3 attn = vec3(attenuation);

    // Ambient
	vec3 ambient = lamp.ambient * vec3(texture(mat.diffuse, vec2(TexCoords.x, 1.0 - TexCoords.y))) * attn;
  	
    // Diffuse 
    vec3 lightDir = normalize(lamp.position - FragPos);
	// Spotlight calculation
	float theta = dot(lightDir, normalize(-lamp.direction));
	float epsilon = lamp.innerCutOff - lamp.outerCutOff;
	float intensity = clamp( (theta - lamp.outerCutOff)/epsilon, 0.0, 1.0 );
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lamp.diffuse * diff * vec3(texture(mat.diffuse, vec2(TexCoords.x, 1.0 - TexCoords.y))) * attn * intensity;  
    
    // Specular
    vec3 halfVec = normalize(viewDir + lightDir);   
    float spec = pow(max(dot(norm, halfVec), 0.0), mat.shininess * 4.0);
    vec3 specular = lamp.specular * spec * vec3(texture(mat.specular, vec2(TexCoords.x, 1.0 - TexCoords.y))) * attn * intensity;  
	
	vec3 emission = vec3(texture(mat.emission, vec2(TexCoords.x, 1.0 - TexCoords.y)));
    
    return ambient + diffuse + specular + emission;
}