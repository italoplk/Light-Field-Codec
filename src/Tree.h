#ifndef LF_CODEC_TREE_H
#define LF_CODEC_TREE_H

#include <vector>
#include <iostream>
#include <cmath>
#include <queue>
#include <fstream>

#include "EncoderParameters.h"
#include "Point4D.h"

#define HEXADECA 16
#define SEP ","
#define IS_ORIGINAL false
#define IS_8X8X8X8 true
#define IS_4X4X4X4 false

using namespace std;

struct Ponto4D{
    float x = 0, y = 0, u = 0, v = 0;
};

struct Hypercube{
    int ****data;
    Point4D dim = {0,0,0,0};

    Hypercube(int x, int y, int u, int v){
        this->dim.x = x;
        this->dim.y = y;
        this->dim.u = u;
        this->dim.v = v;

        this->data = new int***[x];
        for (int i = 0; i < x; ++i) {
            this->data[i] = new int**[y];
            for (int j = 0; j < y; ++j) {
                this->data[i][j] = new int*[u];
                for (int k = 0; k < u; ++k) {
                    this->data[i][j][k] = new int[v];
                    for (int l = 0; l < v; ++l) {
                        this->data[i][j][k][l] = 0;
                    }
                }
            }
        }
    }
};

struct Node{
    Hypercube *hypercube;
    uint n_zero, n_one, n_two, n_greater_than_two, hypercubo_size, total_values;
    vector<Node *> child;

    Node(int x, int y, int u, int v){
        this->hypercube = new Hypercube(x,y,u,v);
    }
    void CountValues(){
        uint zero = 0, one = 0, two = 0, gttow = 0;
        for (int it_v = 0; it_v < this->hypercube->dim.v ; ++it_v) {
            for (int it_u = 0; it_u < this->hypercube->dim.u; ++it_u) {
                for (int it_y = 0; it_y < this->hypercube->dim.y; ++it_y) {
                    for (int it_x = 0; it_x < this->hypercube->dim.x; ++it_x) {
                        if (abs(this->hypercube->data[it_x][it_y][it_u][it_v]) == 0) ++zero;
                        else if (abs(this->hypercube->data[it_x][it_y][it_u][it_v]) == 1) ++one;
                        else if (abs(this->hypercube->data[it_x][it_y][it_u][it_v]) == 2) ++two;
                        else ++gttow;
                    }
                }
            }
        }
        this->SetHypercuboSize(this->hypercube->dim.x * this->hypercube->dim.y * this->hypercube->dim.u * this->hypercube->dim.v);
        this->SetNZero(zero);
        this->SetNOne(one);
        this->SetNTwo(two);
        this->SetNGreaterThanTwo(gttow);
        this->SetTotalValues(zero + one + two + gttow);
    }
    void SetFileValues(ofstream& file, string light_field, uint hypercubo, uint channel, uint level, Ponto4D &pos){
        file <<
             light_field << SEP <<
             hypercubo << SEP <<
             pos.x << SEP <<
             pos.y << SEP <<
             pos.u << SEP <<
             pos.v << SEP <<
             channel << SEP <<
             level << SEP <<
             this->hypercubo_size << SEP <<
             this->n_zero << SEP <<
             this->n_one << SEP <<
             this->n_two << SEP <<
             this->n_greater_than_two << SEP <<
             this->total_values << SEP << endl;
    }
    void SetNZero(uint n_zero){
        this->n_zero = n_zero;
    }
    void SetNOne(uint n_one){
        this->n_one = n_one;
    }
    void SetNTwo(uint n_two){
        this->n_two = n_two;
    }
    void SetNGreaterThanTwo(uint n_gttwo){
        this->n_greater_than_two = n_gttwo;
    }
    void SetHypercuboSize(uint hypercubo_size){
        this->hypercubo_size = hypercubo_size;
    }
    void SetTotalValues(uint total_values){
        this->total_values = total_values;
    }
};

class Tree {
public:
    Tree();

    Node* CreateRoot(int *bitstream, const Point4D &dim_block);
    void CreateTree(Node * root,ofstream& file, string light_field, uint hypercubo, uint channel, uint level, const Point4D &pos, const Point4D &hypercubo_pos, Point4D middle_before);
    void DeleteTree(Node** node_ref);

    ~Tree();

private:
    Node* NewNode(Hypercube *block);
    void get_block_partition(int ****block, int ****data, int start_x, int end_x, int start_y, int end_y, int start_u, int end_u, int start_v, int end_v);
    void _deleteTree(Node* node);
    Ponto4D prox_pos = {0,0,0,0};
    uint size = 0;
};

#endif //LF_CODEC_TREE_H
