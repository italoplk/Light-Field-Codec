#ifndef LF_CODEC_TREE_H
#define LF_CODEC_TREE_H

#include <vector>
#include <iostream>
#include <cmath>
#include <queue>
#include <fstream>

#include "EncoderParameters.h"

#define HEXADECA 16
#define SEP ","

using namespace std;

struct Node{
    vector<int> bitstream;
    uint n_zero, n_one, n_two, n_greater_than_two, hypercubo_size, total_values;
    vector<Node *> child;

    void CountValues(){
        uint zero = 0, one = 0, two = 0, gttow = 0;
        for (auto value : this->bitstream) {
            if (abs(value) == 0) zero++;
            else if (abs(value) == 1) one++;
            else if (abs(value) == 2) two++;
            else gttow++;
        }
        this->SetHypercuboSize(this->bitstream.size());
        this->SetNZero(zero);
        this->SetNOne(one);
        this->SetNTwo(two);
        this->SetNGreaterThanTwo(gttow);
        this->SetTotalValues(zero + one + two + gttow);
    }
    void SetFileValues(ofstream& file, string light_field, uint hypercubo, uint channel, uint level){
        file <<
             light_field << SEP <<
             hypercubo << SEP <<
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

    Node* CreateRoot(int *bitstream, uint nsamples);
    void CreateTree(Node * root,ofstream& file, string light_field, uint hypercubo, uint channel, uint level);
    void DeleteTree(Node *root);

    ~Tree();
    //void PrintLevelOrder();

private:
    Node* NewNode(vector<int> bitstream);

    //void Print(vector<int> const &input);
    //void PrintValues();
};

#endif //LF_CODEC_TREE_H
