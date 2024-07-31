#version 320 es

// 将nv21分离为yuv三个平面。最后分别将yuv三个数据传递给片段着色器。
// 然后在片段着色器内进行yuv转rgb。也可以将转rgb的步骤放在这里。

// nv21，yuv420的一种，每四个Y共用一组VU分量。Y是单独平面，剩下VU分量混编在一个平面
// VU平面的宽度与图像宽度、Y平面宽度一致。高度是一半。

precision highp int;

// 分配本地工作组大小。
// 一个unit一次读取32bit，也就是VUVU，因此一个x为4。
// nv21中uv平面高度是图像高度的一半，所以这里y=4。就可以确保一个local group每次可以处理16x8像素。
layout (local_size_x = 4, local_size_y = 4) in;

// nv21的y平面本来就是独立的，这里只需要分离vu平面。
layout (binding = 0, std430) readonly buffer VU_BUFFER {
    int vuBuffer[];
};

// 输入输出都是32位的。
layout (binding = 1, std430) coherent writeonly  buffer U_BUFFER {
    int uBuffer[];
};

layout (binding = 2, std430) coherent writeonly buffer V_BUFFER {
    int vBuffer[];
};

layout (r32i) shared uniform int image_width;

// uvec3 gl_WorkGroupSize 本地工作组大小，存储的就是local_size_x/y/z
// uvec3 gl_NumWorkGroups 传递给glDispatchCompute()的参数，可以知道全局工作组的大小
// uvec3 gl_LocalInvocationID 当前单元在本地工作组中的位置
// uvec3 gl_WorkGroupID 当前本地工作组在全局工作组中的位置
// uvec3 gl_GlobalInvocationID 当前单元在全局工作组中的位置
// uint gl_LocalInvocationIndex 当前单元在本地工作组中的一维索引


void main(void) {

    int unit_x = int(gl_GlobalInvocationID.x);
    int unit_y = int(gl_GlobalInvocationID.y);

    int offset = unit_x % 2 == 0 ? 0 : 16;

    int t = vuBuffer[unit_y * image_width / 4 + unit_x];

    int v1 = t & 0x00FF;
    int u1 = (t >> 8) & 0x00FF;
    int v2 = (t >> 16) & 0x00FF;
    int u2 = (t >> 24) & 0x00FF;

    int v = (v2 << 8) | v1;
    int u = (u2 << 8) | u1;

    v = v << offset;
    u = u << offset;

    // 这里采用原子操作。
    // uBuffer比uvBuffer的宽度少一半。
    atomicOr(vBuffer[unit_y * image_width / 4 / 2 + unit_x / 2], v);
    atomicOr(uBuffer[unit_y * image_width / 4 / 2 + unit_x / 2], u);
//
//    int oriV = vBuffer[unit_x / 2];
//    int oriU = uBuffer[unit_x / 2];
//
//    u = oriU | u;
//    v = oriV | v;
//    vBuffer[1] = 0x11223344;
//    uBuffer[1] = 0x11223344;

    memoryBarrier();
}
