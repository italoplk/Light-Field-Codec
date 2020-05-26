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

struct Node{
    Hypercube *hypercube;
    uint n_zero = 0,
         n_one = 0,
         n_two = 0,
         n_greater_than_two = 0,
         max_value = 0,
         mean_value = 0, // Absolut mean
         hypercubo_size = 0;
    bool significant_value = false; // false - Haven't significant values | true - Have significant values

    vector<Node *> child;

    Node(int x, int y, int u, int v){
        this->hypercube = new Hypercube(x,y,u,v);
    }

#if HEXADECA_TREE_TYPE == 0
    void CountValues(){
        uint acc = 0, max = 0;
        for (int it_v = 0; it_v < this->hypercube->dim.v ; ++it_v) {
            for (int it_u = 0; it_u < this->hypercube->dim.u; ++it_u) {
                for (int it_y = 0; it_y < this->hypercube->dim.y; ++it_y) {
                    for (int it_x = 0; it_x < this->hypercube->dim.x; ++it_x) {
                        acc += abs(this->hypercube->data[it_x][it_y][it_u][it_v]);
                        if (max < this->hypercube->data[it_x][it_y][it_u][it_v]) {max = this->hypercube->data[it_x][it_y][it_u][it_v];}
                        if (this->hypercube->data[it_x][it_y][it_u][it_v] != 0) {this->significant_value = true;}
                        if (abs(this->hypercube->data[it_x][it_y][it_u][it_v]) == 0) {++this->n_zero;}
                        else if (abs(this->hypercube->data[it_x][it_y][it_u][it_v]) == 1) {++this->n_one;}
                        else if (abs(this->hypercube->data[it_x][it_y][it_u][it_v]) == 2) {++this->n_two;}
                        else {++this->n_greater_than_two;}
                    }
                }
            }
        }
        this->hypercubo_size = this->hypercube->dim.x * this->hypercube->dim.y * this->hypercube->dim.u * this->hypercube->dim.v;
        this->max_value = max;
        this->mean_value = round((float)acc / this->hypercubo_size);
    }
    void SetFileValues(ofstream& file, string light_field, uint hypercubo, uint channel, uint level, Ponto4D &pos){
        string order = "";
        order = (level == 0) ? "Origial" : (level == 1) ? "Order_8" : "Order_4";
        file <<
             light_field << SEP <<
             order << SEP <<
             hypercubo << SEP <<
             channel << SEP <<
             pos.x << SEP <<
             pos.y << SEP <<
             pos.u << SEP <<
             pos.v << SEP <<
             this->hypercubo_size << SEP <<
             this->n_zero << SEP <<
             this->n_one << SEP <<
             this->n_two << SEP <<
             this->n_greater_than_two <<  SEP <<
             this->max_value << SEP <<
             this->mean_value << SEP <<
             this->significant_value << SEP << endl;
    }
#else
    void CountValues(ofstream& file, string light_field, uint hypercubo, uint channel, uint level, Ponto4D &hy_pos){
        uint zero = 0, one = 0, two = 0, gttow = 0;
        Ponto4D position = {0,0,0,0};
        for (int it_v = 0; it_v < this->hypercube->dim.v ; ++it_v) {
            for (int it_u = 0; it_u < this->hypercube->dim.u; ++it_u) {
                for (int it_y = 0; it_y < this->hypercube->dim.y; ++it_y) {
                    for (int it_x = 0; it_x < this->hypercube->dim.x; ++it_x) {
                        position = {float(it_x), float(it_y), float(it_u), float(it_v)};
                        this->SetFileValues(file,light_field,hypercubo,channel,level, hy_pos, position ,abs(this->hypercube->data[it_x][it_y][it_u][it_v]));
                    }
                }
            }
        }
    }

    void SetFileValues(ofstream& file, string light_field, uint hypercubo, uint channel, uint level, Ponto4D &pos, Ponto4D &position, int value){
        file <<
             light_field << SEP <<
             order << SEP <<
             hypercubo << SEP <<
             channel << SEP <<
             pos.x << SEP <<
             pos.y << SEP <<
             pos.u << SEP <<
             pos.v << SEP <<
             position.x << SEP <<
             position.y << SEP <<
             position.u << SEP <<
             position.v << SEP <<
             value << SEP << endl;
    }
#endif
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
    Ponto4D hy_pos = {0,0,0,0};
    uint size = 0;
};

#endif //LF_CODEC_TREE_H
