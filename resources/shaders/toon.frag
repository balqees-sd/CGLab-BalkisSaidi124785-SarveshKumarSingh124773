#version 150

in vec3 pass_Normal, pass_Position, pass_Camera_Position;
in mat4 passViewMatrix;

out vec4 out_Color;

uniform vec3 planetColor;
uniform vec3 light_color;
uniform vec3 light_position;
uniform float light_strength;
uniform float ambient_strength;

void main() {

  vec3 normal = normalize(pass_Normal);
  vec3 specular_color = vec3(1.0,1.0,1.0);

  vec4 outline_color = vec4(1.0,0.0,0.0,1.0);
  float outline_thickness = 0.3;

  float diffuse_reflection_factor = 0.9;
  float specular_reflection_factor = 0.9;
  int n = 32;

  vec3 camera_Position = pass_Camera_Position;
  
  vec3 transformed_light_position = (passViewMatrix * vec4(light_position,1.0)).xyz;

  vec3 light_Direction = normalize(transformed_light_position - pass_Position);
  vec3 view_Direction = normalize(camera_Position - pass_Position);
  vec3 h = normalize(view_Direction + light_Direction);

  float diffuse_light_strength = light_strength * diffuse_reflection_factor * max(dot(normal,light_Direction),0);
  float specular_light_strength = light_strength * specular_reflection_factor * pow(max(dot(h,normal),0),n);
  
  vec3 ambient = ambient_strength * light_color;
  vec3 diffuse = diffuse_light_strength * light_color;
  vec3 specular =  specular_light_strength * specular_color;

  if(abs(dot(normal,view_Direction)) < outline_thickness){
    out_Color = outline_color;
  }
  else{
    if (diffuse_light_strength > 0.95)
      diffuse = 1.0 * light_color;
    else if (diffuse_light_strength > 0.6)
      diffuse = 0.7 * light_color;
    else if (diffuse_light_strength > 0.2)
      diffuse = 0.35 * light_color;
    else 
      diffuse = 0.1 * light_color;

    out_Color = vec4((ambient + diffuse) * planetColor + specular * light_color, 1.0);
  }
}
