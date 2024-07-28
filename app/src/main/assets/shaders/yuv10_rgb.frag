#version 330 core

uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;

in vec2 TexCoord;

out vec4 FragColor;

// see: https://zhuanlan.zhihu.com/p/436186749

void main() {

    // 10bit格式仍然是按2byte存储，但是在高6位补0。在GL将其转换到float时，会除UINT16_MAX，导致各分量都偏小。
    // 因此需要按比例将其放大才会正常。
    float ra = 0x00FFFF * 1.0f / 0x0003FF;

    // 部分色域BT601
    // float y = texture(tex_y, TexCoord).r - 0.0625f;
    // float u = texture(tex_u, TexCoord).r - 0.5f;
    // float v = texture(tex_v, TexCoord).r - 0.5f;

    // float r = 1.164f * y +              1.596f * v;
    // float g = 1.164f * y - 0.391f * u - 0.813f * v;
    // float b = 1.164f * y + 2.018f * u;

    
    // 全色域BT601
    // float y = texture(tex_y, TexCoord).r;
    // float u = texture(tex_u, TexCoord).r - 0.5f;
    // float v = texture(tex_v, TexCoord).r - 0.5f;

    // float r = y + 1.402f * v;
    // float g = y - 0.344f * u - 0.714f * v;
    // float b = y + 1.772 * u;

    // 部分色域BT709
    float y = texture(tex_y, TexCoord).r * ra - 0.0625f;
    float u = texture(tex_u, TexCoord).r * ra - 0.5f;
    float v = texture(tex_v, TexCoord).r * ra - 0.5f;

    float r = 1.164f * y + 1.793f * v;
    float g = 1.164f * y - 0.213f * u - 0.533f * v;
    float b = 1.164f * y + 2.112f * u;

    // 全色域BT709
    // float y = texture(tex_y, TexCoord).r;
    // float u = texture(tex_u, TexCoord).r - 0.5f;
    // float v = texture(tex_v, TexCoord).r - 0.5f;

    // float r = y +              1.280f * v;
    // float g = y - 0.215f * u - 0.381f * v;
    // float b = y + 2.128f * u;

    // float a = texture(tex_v, TexCoord).r;
    FragColor = vec4(r, g, b, 1.0f);
}