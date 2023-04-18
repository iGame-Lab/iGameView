#version 330 
out vec4 FragColor;
in vec3 pos;
in vec3 pos_fixed;
in vec3 normal;
in vec3 normal_fixed;
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
uniform float shininess;
uniform vec3 viewpos;
uniform int rendermode = 3;     // 渲染模式，1：点，2：线，3：光滑面，4：平面
uniform int colormode = 1;      // 颜色模式，1：光照方式，2：根据法向方式，3：自定颜色模式，4：使用传入VBO时使用的值作为颜色（默认传入的是顶点的向量）
uniform vec4 color = vec4(1.0, 0, 0, 1.0);  // 自定颜色
void main() {
    vec3 n;
    if(colormode == 1){     // 光照模式下，物体法向会随着物体旋转而改变
        if(rendermode == 4)
            n = normalize(cross(dFdx(pos),dFdy(pos)));  // 离散
        else
            n = normalize(normal);  // 连续
    }
    else if(colormode == 2){     // 法向模式下，物体法向不会改变
        if(rendermode == 4){
            n = normalize(cross(dFdx(pos_fixed),dFdy(pos_fixed))); 
        } 
        else{
            n = normalize(normal_fixed); 
        }
    }
	
    // ambient
    vec3 ambient = light.ambient;

    // diffuse 
    vec3 lightDir = normalize(light.position - pos);
    float diff = max(dot(n, lightDir), 0.0);
    vec3 diffuse = light.diffuse *diff;
    
    // specular
    vec3 viewDir = normalize(viewpos - pos);
    vec3 reflectDir = reflect(-lightDir, n); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.specular * spec;  

    vec3 result = ambient + diffuse + specular;

      if(colormode == 1){
        switch(rendermode){
        case 1:
        case 2:
            FragColor = vec4(ambient*1.8,1.0);
            break;
        case 3:
        case 4:
            FragColor = vec4(result,1.0);
            break;
        }   
      }
      else if(colormode == 2){
        FragColor = vec4((n+vec3(1.0,1.0,1.0))/2,1.0);  
      }
      else if(colormode == 3){
        FragColor = color;
      }
      else if(colormode == 4){
        FragColor = vec4(normal_fixed, 1.0);
      }
}