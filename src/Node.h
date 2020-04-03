#ifndef LF_CODEC_NODE_H
#define LF_CODEC_NODE_H

#include <vector>
#include <iostream>
#include <cmath>
#include <queue>
#include <fstream>

#include "EncoderParameters.h"

#define HEXADECA 16
#define SEP ","

using namespace std;

class Node {
public:
    Node(int *bitstream, uint nsamples);
    Node(vector<int> bitstream);

    ~Node();

    void CreateTree(ofstream& file, string light_field, uint hypercubo, uint channel, uint level);
    void PrintLevelOrder();

private:
    vector<int> bitstream;
    uint n_zero, n_one, n_two, n_greater_than_two, hypercubo_size, total_values;
    vector<Node *> child;

    void CountValues();
    void SetFileValues(ofstream& file, string light_field, uint hypercubo, uint channel, uint level);
    void SetNZero(uint n_zero);
    void SetNOne(uint n_one);
    void SetNTwo(uint n_two);
    void SetNGreaterThanTwo(uint n_gttwo);
    void SetHypercuboSize(uint hypercubo_size);
    void SetTotalValues(uint total_values);

    void Print(vector<int> const &input);
    void PrintValues();
};

#endif //LF_CODEC_NODE_H
