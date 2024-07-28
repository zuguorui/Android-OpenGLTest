#version 330 core

/**
测试GLSL语言的输入和输出
*/

// 顶点着色器的输入，location定义了该变量的index。在设置顶点属性glVertexAttribPointer时需要的就是这个index。
// 也可以省略，使用glGetAttribLocation(GLuint program, const GLchar name)来通过名字查询属性的index值。
layout (location = 0) in vec3 mPos;

// 顶点着色器的输出，两个着色器之间如果有类型和名称完全相同的输入输出，那么编译器就会把它们链接起来。
// 这里的输出属性mColor在input_output.frag中有名称与类型完全相同的输入属性。编译器就会将顶点着色器的输出
// mColor传递给片段着色器作为输入。
out vec4 mColor;

void main() {
    gl_Position = vec4(mPos, 1.0);
    mColor = vec4(mPos, 1.0);
}
