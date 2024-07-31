#version 320 es

// 将nv21分离重排为yuvXyuvX数据的计算着色器。这里其实是直接将结果的yuv看作rgb数据传递给片段着色器。
// 然后在片段着色器内进行yuv转rgb。也可以将转rgb的步骤放在这里。

// nv21，yuv420的一种，每四个Y共用一组UV分量。Y是单独平面，剩下UV分量混编在一个平面

precision highp int;

/**
分配本地工作组大小。
nv21平面分布如下：
第一行:      Y Y | Y Y
第一二行共用: V U | V U
第二行:      Y Y | Y Y
y和uv宽度相同，uv的高度是y的一半。

buffer按32bit读取，这里为了使着色器代码简洁一些，我选择一次处理2个纹素。因此根据x的奇偶性，选择输入的高16位或者低16位。
一般视频的宽度是16的整数倍，而高度基本都是8的整数倍。所以这里设置local group的size为8x8，即一个local group处理
16x8像素，外部dispatch(imageWidth / 16, imageHeight / 8, 1)。
*/
layout (local_size_x = 8, local_size_y = 8) in;

layout (binding = 0, std430) readonly buffer Y_BUFFER {
    int yBuffer[];
};

layout (binding = 1, std430) readonly buffer VU_BUFFER {
    int vuBuffer[];
};

// 输入输出都是32位的。yuv都是8位，最后一个x不用。
layout (binding = 2, std430) writeonly buffer YUVX_BUFFER {
    int yuvxBuffer[];
};

layout (r32i) shared uniform int width;
layout (r32i) shared uniform int height;

// uvec3 gl_WorkGroupSize 本地工作组大小，存储的就是local_size_x/y/z
// uvec3 gl_NumWorkGroups 传递给glDispatchCompute()的参数，可以知道全局工作组的大小
// uvec3 gl_LocalInvocationID 当前单元在本地工作组中的位置
// uvec3 gl_WorkGroupID 当前本地工作组在全局工作组中的位置
// uvec3 gl_GlobalInvocationID 当前单元在全局工作组中的位置
// uint gl_LocalInvocationIndex 当前单元在本地工作组中的一维索引

#define GET_YUVX(Y, U, V) ( ((V & 0x00FF) << 16) | ((U & 0x00FF) << 8) | (Y & 0x00FF) )

void main(void) {

    // unit_x取值[0, width/2 - 1]，unit_y取值[0, height - 1]
    int unit_x = int(gl_GlobalInvocationID.x);
    int unit_y = int(gl_GlobalInvocationID.y);

    // 需要将unit的位置转换为int32数组的序号。例如宽度是width，y是byte类型，则一行数据转换为int32的个数为
    // width / 4
    int y_x = unit_x / 2;
    int y_y = unit_y;

    int vu_x = unit_x / 2;
    int vu_y = unit_y / 2; // 两行y对应一行vu

    // 位移按小尾序
    int offset = unit_x % 2 == 0 ? 0 : 16;

    int y = yBuffer[y_y * width / 4 + y_x];
    int vu = vuBuffer[vu_y * width / 4 + vu_x];

    y = (y >> offset) & 0x00FFFF;
    vu = (vu >> offset) & 0x00FFFF;

    int y1 = y & 0x00FF;
    int y2 = (y >> 8) & 0x00FF;

    int u = (vu >> 8) & 0x00FF;
    int v = vu & 0x00FF;

    int yuvx1 = GET_YUVX(y1, u, v);
    int yuvx2 = GET_YUVX(y2, u, v);

    yuvxBuffer[unit_y * width + 2 * unit_x] = yuvx1;
    yuvxBuffer[unit_y * width + 2 * unit_x + 1] = yuvx2;

    memoryBarrier();
}
