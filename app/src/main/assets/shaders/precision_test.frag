#version 320 es

// 指定精度对buffer无效，buffer总是按32位。
precision mediump int;

layout (local_size_x = 10) in;

layout (binding = 0, std430) buffer Input {
    int inputBuffer[];
};

layout (binding = 1, std430) buffer Output {
    mediump int outputBuffer[];
};

void main() {
    int x = int(gl_GlobalInvocationID.x);

    highp int t = inputBuffer[x];
    //t = t & 0xFFFF0000;
    //t += 1;
    //t = 0x33445566;
    outputBuffer[x] = t;
}
