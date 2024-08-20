#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

in vec3 Normal;
in vec3 fragPos;
in vec3 camPos;
in vec4 fragPosLight;

const int NR_POINT_LIGHTS = 2;
const float REFLECT_AMOUNT = 1.0f;

struct str_point_light
{
   vec3 position; //16 bytes
   vec4 color; //16 bytes
   float linear; //4 bytes
   float quadratic; //4 bytes
   float constant;   //4 bytes
};

layout(std140) uniform Lights 
{
   str_point_light[NR_POINT_LIGHTS] lights;
};
//in str_point_light[1] lights;

//layout(std140) uniform Lights 
//{
//   str_point_light lights[1];
//};

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D shadowMap;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 dir_light_pos;
uniform vec4 dir_light_color;

uniform samplerCube skybox;

//uniform vec3 camPos;

vec4 point_light(int i) {

   vec3 light_vector = lights[i].position - fragPos;
   float light_distance = length(light_vector);

   //float quadratic = 0.1f;
   //float linear = 0.3f;
   //float constant = 1.0f;
   float intensity = 1.0f / (lights[i].quadratic * pow(light_distance, 2) + lights[i].linear * light_distance + lights[i].constant);

   float ambient = 0.0f;
   vec3 normal = normalize(-Normal);
   vec3 lightDir = normalize(light_vector);

   float diffuse = max(dot(normal, lightDir), 0.0f);

   float specularLight = 0.0f;
   vec3 viewDir = normalize(camPos - fragPos);
   vec3 reflectDir = reflect(-lightDir, normal);
   float specAmount = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
   float specular = specularLight * specAmount;

   return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lights[i].color;
   
   }

vec4 spot_light() {

   float outer_cone = 0.90f;
   float inner_cone = 0.95f;

   vec3 light_vector = lightPos - fragPos;

   float ambient = 0.2f;
   vec3 normal = normalize(Normal);
   vec3 lightDir = normalize(light_vector);

   float diffuse = max(dot(normal, lightDir), 0.0f);

   float specularLight = 0.50f;
   vec3 viewDir = normalize(camPos - fragPos);
   vec3 reflectDir = reflect(-lightDir, normal);
   float specAmount = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
   float specular = specularLight * specAmount;

   float theta = dot(vec3(0.0f, -1.0f, 0.0f), -lightDir);
   float intensity = clamp((theta - outer_cone) / (inner_cone - outer_cone), 0.0f, 1.0f);

   return (texture(diffuse0, texCoord) * (diffuse * intensity + ambient) + texture(specular0, texCoord).r * specular * intensity) * lightColor;
   }

vec4 directional_light() {
   float ambient = 0.3f;
   vec3 normal = normalize(-Normal);
   vec3 lightDir = normalize(dir_light_pos);

   float diffuse = max(dot(normal, lightDir), 0.0f);

   float specular = 0.0f;
   if (diffuse != 0.0f) {
      float specularLight = 0.3f;
      vec3 viewDir = normalize(camPos - fragPos);
      vec3 reflectDir = reflect(-lightDir, normal);
      float specAmount = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
      specular = specularLight * specAmount;
   }

   float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
      float closestDepth = texture(shadowMap, lightCoords.xy).r;
		float currentDepth = lightCoords.z;
      if(currentDepth > closestDepth) {
         shadow = 1.0f;
      }
		// Prevents shadow acne
		float bias = max(0.007f * (1.0f - dot(normal, lightDir)), 0.01f);

		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		      float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;  
		    }    
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);
	}
   
   

   return (texture(diffuse0, texCoord) * (diffuse * (1.0f - shadow) + ambient) + texture(specular0, texCoord).r * specular  * (1.0f - shadow)) * dir_light_color;
   //return texture(shadowMap, lightCoords.xy);
   //return texture(diffuse0, texCoord);
   }

void main()
{

   vec3 viewDir = normalize(fragPos - camPos);
   vec3 reflectDir = reflect(viewDir, normalize(Normal));
   vec4 envColor = vec4(texture(skybox, reflectDir).rgb, 1.0f) * REFLECT_AMOUNT;

   vec4 c = vec4(0.0f, 0.0f, 0.0f, 1.0f);

   for (int i = 0; i < NR_POINT_LIGHTS; i++)
   {
      c += point_light(i);

         
   }
   c += directional_light();
   c += envColor;
   c.r = clamp(c.r, 0.0f, 1.0f);
   c.g = clamp(c.g, 0.0f, 1.0f);
   c.b = clamp(c.b, 0.0f, 1.0f);
   c.a = clamp(c.a, 0.0f, 1.0f);
   if(texture(diffuse0, texCoord).a < 0.1f)
   {
      discard;
   }
   FragColor = envColor;
   //FragColor
   //FragColor = directional_light();
   
	//FragColor = point_light();
}