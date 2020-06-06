#ifndef LF_CODEC_TREE_H
#define LF_CODEC_TREE_H

#include <vector>
#include <iostream>
#include <cmath>
#include <queue>
#include <fstream>

#include "EncoderParameters.h"
#include "Point4D.h"
#include "Typedef.h"

#define HEXADECA 16
#define SEP ","

using namespace std;

struct Point_4D{
    int x = 0, y = 0, u = 0, v = 0;
};

struct LF_Props{
    ofstream file;
    string light_field_name;
    uint hypercubo,
         channel;

    LF_Props(ofstream &file, string light_field, uint hypercubo, uint channel){
        this->file = dynamic_cast<basic_ofstream<char> &&>(file);
        this->light_field_name = light_field;
        this->hypercubo = hypercubo;
        this->channel = channel;
    }
};

struct Hypercube{
    int ****data;
    Point_4D dim = {0,0,0,0};

    Hypercube(int x, int y, int u, int v){
        this->dim.x = x;
        this->dim.y = y;
        this->dim.u = u;
        this->dim.v = v;

        this->data = (int ****) calloc(x, sizeof(int ***));
        for (int i = 0; i < x; ++i) {
            this->data[i] = (int ***) calloc(y, sizeof(int **));
            for (int j = 0; j < y; ++j) {
                this->data[i][j] = (int **) calloc(u, sizeof(int *));
                for (int k = 0; k < u; ++k) {
                    this->data[i][j][k] = (int *) calloc(v, sizeof(int));
                }
            }
        }
    }
};

struct Attributes{
    uint n_zero = 0,
         n_one = 0,
         n_two = 0,
         n_greater_than_two = 0,
         hypercubo_size = 0;

    int max_value = 0;

    float mean_value = 0; // Absolut mean

    bool significant_value = false; // false - Haven't significant values | true - Have significant values
};

struct Node{
    Point_4D start{0,0,0,0};
    Point_4D end{0,0,0,0};
    Point_4D hypercube_dim{0,0,0,0};

    Attributes *att;

    vector<Node *> child;

    Node(Point_4D start, Point_4D end, Point_4D hypercube_dim){
        this->start = start;
        this->end = end;
        this->hypercube_dim = hypercube_dim;

        this->att = nullptr;

        for (int i = 0; i < HEXADECA; ++i) {
            this->child.push_back(nullptr);
        }
    }

    void SetAttributes(Attributes *att){
        this->att = att;
    }
};

class Tree {
public:
    Tree();
    Node* CreateRoot(ofstream& file, string light_field, uint hypercubo, uint channel, int *bitstream, const Point4D &dim_block);
    void CreateTree(Node * root, uint level, const Point4D &pos, const Point_4D &hypercubo_pos, Point_4D middle_before);
    void DeleteTree(Node** node_ref);

    ~Tree();

private:
    LF_Props *props;
    Hypercube *hypercube = nullptr;

    Point_4D next_start_position = {0,0,0,0};
    Point_4D next_end_position = {0,0,0,0};
    Point_4D hy_pos = {0,0,0,0};

    uint size = 0;

    void ComputeAttributes(Node *node, int start_x, int end_x, int start_y, int end_y, int start_u, int end_u, int start_v, int end_v);
    void _deleteTree(Node* node);
    void HypercubePosition(Point_4D *middle);

    void WriteAttributesInFile(uint level, Point_4D &pos, Node* node);
    void WriteValuesInFile(uint level, Point_4D &pos,Point_4D &position, int value);

    void ComputePositions(Point_4D start, Point_4D middle_before, Point_4D middle);
    void ComputeValues(Node *node, int start_x, int end_x, int start_y, int end_y, int start_u, int end_u, int start_v, int end_v, uint level, Point_4D &pos);

    Point_4D ComputeStart(int index, Point_4D middle);
};

#endif //LF_CODEC_TREE_H
