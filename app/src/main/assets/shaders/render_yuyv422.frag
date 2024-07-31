#version 320 es

/*
直接渲染YUYV422数据，这个转换并不精确，会造成画面模糊。
如果把一组YUYV当成RGBA给GL一起采样，那么Y的采样就会错误。
如果把YU和YV分别当做RG给GL分别采样，那么U和V的采样机会错误。
*/

precision mediump float;

uniform sampler2D yuyv_tex;


in vec2 TexCoord;
out vec4 FragColor;

void main() {

    vec4 yuyv = texture(yuyv_tex, TexCoord);

    float y = (yuyv.r + yuyv.b) / 2.0f;
//    if (int(TexCoord.x) % 2 == 0) {
//
//    }
    float u = yuyv.g;
    float v = yuyv.a;

    float y1 = y;
    float u1 = u;
    float v1 = v;

    y -= 0.0625f;
    u -= 0.5f;
    v -= 0.5f;

    float r = 1.164f * y + 1.793f * v;
    float g = 1.164f * y - 0.213f * u - 0.533f * v;
    float b = 1.164f * y + 2.112f * u;

    FragColor = vec4(r, g, b, 1.0f);
}


