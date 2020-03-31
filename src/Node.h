#ifndef LF_CODEC_NODE_H
#define LF_CODEC_NODE_H

#include <vector>
#include <iostream>
#include <cmath>
#include <queue>

#define HEXADECA 16

using namespace std;

class Node {
public:
    Node(int *bitstream, uint nsamples);
    Node(vector<int> bitstream);

    ~Node();

    void CreateTree();
    void printLevelOrder();

private:
    vector<int> bitstream;
    uint n_zero, n_one, n_two, n_greater_than_two;
    vector<Node *> child;

    void CountValues();
    void SetNZero(int n_zero);
    void SetNOne(int n_one);
    void SetNTwo(int n_two);
    void SetNGreaterThanTwo(int n_gttwo);

    void print(vector<int> const &input);
    void printValues();
};

#endif //LF_CODEC_NODE_H
