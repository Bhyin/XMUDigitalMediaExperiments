//
// Created by Bhyin on 4/30/2023.
//

#ifndef COMPUTERGRAPHIC_UTILS_H
#define COMPUTERGRAPHIC_UTILS_H

const double pi = 3.14159265357989;
const double tau = 2 * pi;
const double degree = pi / 180.0;

struct Material {
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;
};

struct Lighting {
    float ambient[4];
    float diffuse[4];
    float specular[4];
};

void Init(int argc, char **argv);

int Window(
        const char *title,
        int width = 1280,
        int height = 720,
        int x = 0,
        int y = 0
);

bool Enable(unsigned int feature);

void VertexPointer(int size, unsigned int type, int stride, const void *pointer);

void ColorPointer(int size, unsigned int type, int stride, const void *pointer);

void NormalPointer(unsigned int type, int stride, const void *pointer);

void DrawElements(unsigned int primitive, int count, unsigned int type, const void *indices);

void Ortho(double l, double r, double b, double t, double n, double f);

void LookAt(double ex, double ey, double ez,
            double ax, double ay, double az,
            double ux, double uy, double uz);

void ClearColor(float r, float g, float b, float a = 1.0);

void SetMaterial(const Material &material);

void SetLighting(const Lighting &lighting);

#endif //COMPUTERGRAPHIC_UTILS_H
