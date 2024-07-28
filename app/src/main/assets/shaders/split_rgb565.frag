#version 320 es

// 将rgb565分离为8bit rgb数据的计算着色器

precision lowp uint;

// 分配本地工作组大小。图像尺寸一般是16的整数倍。
// 但是OpenGL ES计算着色器只支持最小32位数字，因此一个work unit处理两个纹素，x减半为8。
layout (local_size_x = 8, local_size_y = 16) in;

//layout (binding = 0, r32ui) uniform highp uimageBuffer rawData;
//layout (binding = 1, r32ui) uniform highp uimageBuffer rData;
//layout (binding = 2, r32ui) uniform highp uimageBuffer gData;
//layout (binding = 3, r32ui) uniform highp uimageBuffer bData;

layout (binding = 0, std430) readonly buffer RAW_BUFFER {
    uint rawBuffer[];
};

layout (binding = 1, std430) writeonly buffer RED_BUFFER {
    uint redBuffer[];
};

layout (binding = 2, std430) writeonly buffer GREEN_BUFFER {
    uint greenBuffer[];
};

layout (binding = 3, std430) writeonly buffer BLUE_BUFFER {
    uint blueBuffer[];
};

layout (r32ui) shared uniform lowp uint width;
layout (r32ui) shared uniform lowp uint height;

// uvec3 gl_WorkGroupSize 本地工作组大小，存储的就是local_size_x/y/z
// uvec3 gl_NumWorkGroups 传递给glDispatchCompute()的参数，可以知道全局工作组的大小
// uvec3 gl_LocalInvocationID 当前单元在本地工作组中的位置
// uvec3 gl_WorkGroupID 当前本地工作组在全局工作组中的位置
// uvec3 gl_GlobalInvocationID 当前单元在全局工作组中的位置
// uint gl_LocalInvocationIndex 当前单元在本地工作组中的一维索引

const int R_MASK = 0x00F8;
const int G_MASK = 0x00FC;
const int B_MASK = 0x00F8;

const int R_SHIFT = 8;
const int G_SHIFT = 3;
const int B_SHIFT = 3;

void main(void) {

//    int localGroupIndex = int(gl_WorkGroupID.y * gl_NumWorkGroups.x + gl_WorkGroupID.x);
//    int unitIndex = localGroupIndex * int(gl_WorkGroupSize.x * gl_WorkGroupSize.y) + int(gl_LocalInvocationIndex);
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint unitIndex = y * width + x;

    uint t = rawBuffer[unitIndex];
    uint r = (t >> R_SHIFT) & uint(R_MASK);
    uint g = (t >> G_SHIFT) & uint(G_MASK);
    uint b = (t << B_SHIFT) & uint(B_MASK);

    redBuffer[unitIndex] = uint(0x12345678);
    greenBuffer[unitIndex] = g;
    blueBuffer[unitIndex] = b;


    memoryBarrier();
}
