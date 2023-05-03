//
// Created by Bhyin on 4/30/2023.
//

#ifndef COMPUTERGRAPHIC_FILE_OPS_H
#define COMPUTERGRAPHIC_FILE_OPS_H

class PlyModel {

public:
    PlyModel();

    ~PlyModel();

    void Load(const char *filename);

    inline int VertexNum() const { return vertex_num; }

    inline int FaceNum() const { return face_num; }

    inline double *GetVertices() { return vertices; }

    inline unsigned int *GetFaces() { return faces; }

private:
    int vertex_num;;
    int face_num;
    double *vertices;
    unsigned int *faces;
};

#endif //COMPUTERGRAPHIC_FILE_OPS_H
