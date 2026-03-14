#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aVel;
out vec3 ourColor;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);

    float angle = atan(aVel.y, aVel.x);
    float hue = (angle + 3.14159) / (2.0 * 3.14159);

    ourColor = vec3(
        abs(hue * 6.0 - 3.0) - 1.0,
        2.0 - abs(hue * 6.0 - 2.0),
        2.0 - abs(hue * 6.0 - 4.0)
    );

    ourColor = clamp(ourColor, 0.0, 1.0);
}