#version 320 es

// 将rgb565分离为rgba8888数据的计算着色器

precision highp int;
//precision highp float;

// 分配本地工作组大小。图像宽度一般是16的整数倍。
// 这里int精度是highp，为32位。所有buffer都被看作是int32数组。所以rgb565一次读取的是2个纹素。
// 因此一个work unit处理2x1纹素，宽度减半为8。一个local group处理16x8纹素。
// 之所以要搞成16x8，是因为1920x1080这个主流分辨率的高度不是16的整数倍。
// 外部dispatch仍然是(width/16, height/8, 1)。
// 也许不声明上面的precision，而是为每个变量单独声明精度，可能会使rawBuffer为16位数组。但是试了一下并没有用。
layout (local_size_x = 8, local_size_y = 8) in;

layout (binding = 0, std430) readonly buffer RAW_BUFFER {
    int rawBuffer[];
};

layout (binding = 1, std430) writeonly buffer RGBA_BUFFER {
    int rgbaBuffer[];
};

layout (r32i) shared uniform int width;
layout (r32i) shared uniform int height;

// uvec3 gl_WorkGroupSize 本地工作组大小，存储的就是local_size_x/y/z
// uvec3 gl_NumWorkGroups 传递给glDispatchCompute()的参数，可以知道全局工作组的大小
// uvec3 gl_LocalInvocationID 当前单元在本地工作组中的位置
// uvec3 gl_WorkGroupID 当前本地工作组在全局工作组中的位置
// uvec3 gl_GlobalInvocationID 当前单元在全局工作组中的位置
// uint gl_LocalInvocationIndex 当前单元在本地工作组中的一维索引

#define GET_R(X) int((X >> 8) & 0x00F8)
#define GET_G(X) int((X >> 3) & 0x00FC)
#define GET_B(X) int((X << 3) & 0x00F8)

void main(void) {

//    int x = int(gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x);
//    int y = int(gl_WorkGroupID.y * gl_WorkGroupSize.y + gl_LocalInvocationID.y);

    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);

    // 注意索引，raw的宽度减半了。x的取值范围是[0, width / 2]
    int t = rawBuffer[y * width / 2 + x];

    int tl = t & 0x00FFFF;
    int th = (t >> 16) & 0x00FFFF;

    int rl = GET_R(tl);
    int gl = GET_G(tl);
    int bl = GET_B(tl);

    int rh = GET_R(th);
    int gh = GET_G(th);
    int bh = GET_B(th);

    int rgba_l = ((bl & 0x00FF) << 16) | ((gl & 0x00FF) << 8) | (rl & 0x00FF);
    int rgba_h = ((bh & 0x00FF) << 16) | ((gh & 0x00FF) << 8) | (rh & 0x00FF);

    // 同样注意这里的索引，目标buffer按int32计，则宽高都没有变化。
    // 但是这里x的取值范围减半，所以这里x2。
    rgbaBuffer[y * width + 2 * x] = rgba_l;
    rgbaBuffer[y * width + 2 * x + 1] = rgba_h;

    memoryBarrier();
}
