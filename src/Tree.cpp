#include "Tree.h"

Tree::Tree() {
}

Tree::~Tree() {
}

Node* Tree::CreateRoot(int *bitstream, uint nsamples) {
    Node *root = new Node;
    for (int i = 0; i < nsamples; ++i) {
        root->bitstream.push_back(bitstream[i]);
    }
    for (int j = 0; j < HEXADECA; ++j) {
        root->child.push_back(nullptr);
    }
    this->x = this->y = this->u = this->v = 0;
    return root;
}

Node* Tree::NewNode(vector<int> bitstream) {
    Node *node = new Node();
    node->bitstream = bitstream;
    for (int i = 0; i < HEXADECA; ++i) {
        node->child.push_back(nullptr);
    }
    return node;
}

void Tree::CreateTree(Node* root, ofstream& file, string light_filed, uint hypercubo, uint channel, uint level, const Point4D &pos, const Point4D &hypercubo_pos) {
    if (IS_ORIGINAL)
        this->size = root->bitstream.size();
    else if (IS_4X4X4X4)
        this->size = 4*4*4*4;
    else if (IS_8X8X8X8)
        this->size = 8*8*8*8;
    else return;

    if (root->bitstream.size() <= this->size) {
        root->CountValues();

        if (IS_ORIGINAL){
            this->prox_pos.x = (float)pos.x;
            this->prox_pos.y = (float)pos.y;
            this->prox_pos.u = (float)pos.u;
            this->prox_pos.v = (float)pos.v;
        }
        else if (IS_4X4X4X4){
            this->prox_pos.x = this->x + (16 * hypercubo_pos.x);
            this->prox_pos.y = this->y + (16 * hypercubo_pos.y);
            this->prox_pos.u = this->u + (16 * hypercubo_pos.u);
            this->prox_pos.v = this->v + (16 * hypercubo_pos.v);
        }
        else if (IS_8X8X8X8){ //TODO
            this->prox_pos.x = (float)pos.x;
            this->prox_pos.y = (float)pos.y;
            this->prox_pos.u = (float)pos.u;
            this->prox_pos.v = (float)pos.v;
        }
        else if (IS_16x16x16x16){ //TODO
            this->prox_pos.x = (float)pos.x;
            this->prox_pos.y = (float)pos.y;
            this->prox_pos.u = (float)pos.u;
            this->prox_pos.v = (float)pos.v;
        }
        root->SetFileValues(file, light_filed, hypercubo, channel, level, prox_pos);

        if (IS_4X4X4X4) {
            this->x++;
            if (this->x == 16) {
                this->y++;
                this->x = 0;
            }
            if (this->y == 16)
                this->y = 0;
        }
        return;
    }
    else{
        int n = ceil((double)root->bitstream.size()/HEXADECA);
        vector<int> vec[HEXADECA];
        for (int i = 0; i < HEXADECA; ++i) {
            auto start_itr = next(root->bitstream.cbegin(), i*n);
            auto end_itr = next(root->bitstream.cbegin(), i*n + n);
            vec[i].resize(n);

            if(i*n + n > root->bitstream.size()){
                end_itr = root->bitstream.cend();
                vec[i].resize(root->bitstream.size() - i*n);
            }

            copy(start_itr, end_itr, vec[i].begin());
        }
        uint next_level = level + 1;
        for (int j = 0; j < root->child.size(); ++j) {
            root->child[j] = NewNode(vec[j]);
            this->CreateTree(root->child[j], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos);
        }
    }
}

void Tree::DeleteTree(Node *root) {
    if (root != nullptr){
        for (int i = 0; i < root->child.size(); ++i) {
            this->DeleteTree(root->child[i]);
            free(root->child[i]);
        }
    }
}

/*void Tree::PrintLevelOrder() {
    if (this == NULL) return;
    queue<Tree *> q;
    q.push(this);
    while (!q.empty()){
        uint nodeCount = q.size();
        while (nodeCount > 0){
            Tree *node = q.front();
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

void Tree::PrintValues() {
    cout << "Bitstaream_Size: " << this->bitstream.size() << endl;
    cout << "N_Zero: " << this->n_zero << endl;
    cout << "N_One: " << this->n_one << endl;
    cout << "N_Two: " << this->n_two << endl;
    cout << "N_Greater_Than_Two: " << this->n_greater_than_two << endl;
    cout << "Total_Values: " << this->n_zero + this->n_one + this->n_two + this->n_greater_than_two << "\n" << endl;
}

void Tree::Print(vector<int> const &input){
    for (int i = 0; i < input.size(); i++) {
        cout << input.at(i) << ' ';
    }
    cout << endl;
}*/