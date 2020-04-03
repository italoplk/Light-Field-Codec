#include "Node.h"

Node::Node(int *bitstream, uint nsample){
    for (int i = 0; i < nsample; ++i) {
        this->bitstream.push_back(bitstream[i]);
    }
    for (int j = 0; j < HEXADECA; ++j) {
        this->child.push_back(NULL);
    }
}

Node::Node(vector<int> bitstream){
    this->bitstream = bitstream;
    for (int j = 0; j < HEXADECA; ++j) {
        this->child.push_back(NULL);
    }
}

Node::~Node() {
}

void Node::SetFileValues(ofstream& file, string light_field, uint hypercubo, uint channel, uint level) {
    file <<
            light_field << SEP <<
            channel << SEP <<
            hypercubo << SEP <<
            level << SEP <<
            this->hypercubo_size << SEP <<
            this->n_zero << SEP <<
            this->n_one << SEP <<
            this->n_two << SEP <<
            this->n_greater_than_two << SEP <<
            this->total_values << SEP << endl;
}

void Node::CreateTree(ofstream& file, string light_filed, uint hypercubo, uint channel, uint level) {
    if (this->bitstream.size() < (4*4*4*4)) {
        this->CountValues();
        this->SetFileValues(file, light_filed, hypercubo, channel, level);
        return;
    }
    else{
        this->CountValues();
        this->SetFileValues(file, light_filed, hypercubo, channel, level);
        int n = ceil((double)this->bitstream.size()/HEXADECA);
        vector<int> vec[HEXADECA];
        for (int i = 0; i < HEXADECA; ++i) {
            auto start_itr = next(this->bitstream.cbegin(), i*n);
            auto end_itr = next(this->bitstream.cbegin(), i*n + n);
            vec[i].resize(n);

            if(i*n + n > this->bitstream.size()){
                end_itr = this->bitstream.cend();
                vec[i].resize(this->bitstream.size() - i*n);
            }

            copy(start_itr, end_itr, vec[i].begin());
        }
        uint next_level = level + 1;
        for (int j = 0; j < this->child.size(); ++j) {
            this->child[j] = new Node(vec[j]);
            this->child[j]->CreateTree(file, light_filed, hypercubo, channel, next_level);
        }
    }
}

void Node::CountValues() {
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

void Node::PrintLevelOrder() {
    if (this == NULL) return;
    queue<Node *> q;
    q.push(this);
    while (!q.empty()){
        uint nodeCount = q.size();
        while (nodeCount > 0){
            Node *node = q.front();
            node->PrintValues();
            q.pop();
            for (int i = 0; i < this->child.size(); ++i) {
                if (this->child[i] != NULL)
                    q.push(this->child[i]);
            }
            nodeCount--;
        }
        cout << endl;
    }
}

void Node::PrintValues() {
    cout << "Bitstaream_Size: " << this->bitstream.size() << endl;
    cout << "N_Zero: " << this->n_zero << endl;
    cout << "N_One: " << this->n_one << endl;
    cout << "N_Two: " << this->n_two << endl;
    cout << "N_Greater_Than_Two: " << this->n_greater_than_two << endl;
    cout << "Total_Values: " << this->n_zero + this->n_one + this->n_two + this->n_greater_than_two << "\n" << endl;
}

void Node::SetNZero(uint n_zero) {
    this->n_zero = n_zero;
}

void Node::SetNOne(uint n_one) {
    this->n_one = n_one;
}

void Node::SetNTwo(uint n_two) {
    this->n_two = n_two;
}

void Node::SetNGreaterThanTwo(uint n_gttwo) {
    this->n_greater_than_two = n_gttwo;
}

void Node::SetHypercuboSize(uint hypercubo_size) {
    this->hypercubo_size = hypercubo_size;
}

void Node::SetTotalValues(uint total_values) {
    this->total_values = total_values;
}

void Node::Print(vector<int> const &input){
    for (int i = 0; i < input.size(); i++) {
        cout << input.at(i) << ' ';
    }
    cout << endl;
}