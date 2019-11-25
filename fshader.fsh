uniform sampler2D u_texture;
uniform highp vec4 u_lightPosition;
uniform highp float u_lightPower;
varying highp vec4 v_position;
varying highp vec2 v_texcoord;
varying highp vec3 v_normal;

void main(void)
{
//    vec4 resultColor = vec4(0.0, 0.0, 0.0, 0.0); //Вектор итогового цвета - черный по умолчанию
//    vec4 eyePosition = vec4(0.0, 0.0, 0.0, 1.0); //Позиция взгляда
//    vec4 diffMatColor = texture2D(u_texture, v_texcoord); //Дифьюзный цвет материала
//    vec3 eyeVect = normalize(v_position.xyz - eyePosition.xyz); //Вектор направления взгляда (позиция - вектор где находиться наблюдатель)
//    vec3 lightVect = normalize(v_position.xyz - u_lightPosition); //Вектор цвета(из 000 в точку, которую обрабатывает шейдер)
//    vec3 reflectLight = normalize(reflect(lightVect, v_normal));//Отраженный свет(направление света и нормаль вдоль которой нужно отражать свет)
//    float len = length(v_position.xyz - eyePosition.xyz); //Длинна вектора взгляда
//    float specularFactor = 60.0;//Пятно блика
//    float ambientFactor = 0.1;//Насколько сильно будет светиться материал

//    //                                           неотрицательное скалярное произведение   1 + любой коэф деление выполняется для эффекта затухания света
//    vec4 diffColor = diffMatColor * u_lightPower * max(0.0, dot(v_normal, -lightVect));// / (1.0 + 0.25 * pow(len, 2));
//    gl_FragColor += diffColor;
//    vec4 ambientColor = ambientFactor * diffColor;
//    resultColor += ambientColor;
//    vec4 specularColor = vec4(1.0, 1.0, 1.0, 1.0) * u_lightPower * pov(max(0.0, dot(reflectLight, -eyeVect)), specularFactor) // / (1.0 + 0.25 * pow(len, 2));
//    resultColor += specularColor;
//    gl_FragColor = resultColor;
    gl_FragColor = texture2D(u_texture, v_texcoord);
}
