#include "Tree.h"

Tree::Tree() {
}

Tree::~Tree() {
}

Node* Tree::CreateRoot(int *bitstream, const Point4D &dim_block) {
    Node *root = new Node(dim_block.x, dim_block.y, dim_block.u, dim_block.v);
    int i=0;
    for (int it_v = 0; it_v < root->hypercube->dim.v; ++it_v) {
        for (int it_u = 0; it_u < root->hypercube->dim.u; ++it_u) {
            for (int it_y = 0; it_y < root->hypercube->dim.y; ++it_y) {
                for (int it_x = 0; it_x < root->hypercube->dim.x; ++it_x) {
                    root->hypercube->data[it_x][it_y][it_u][it_v] = bitstream[i];
                    i++;
                }
            }
        }
    }
    for (int j = 0; j < HEXADECA; ++j) {
        root->child.push_back(nullptr);
    }
    return root;
}



Node* Tree::NewNode(Hypercube *block) {
    Node *node = new Node(block->dim.x, block->dim.y, block->dim.u, block->dim.v);
    node->hypercube =  block;
    for (int i = 0; i < HEXADECA; ++i) {
        node->child.push_back(nullptr);
    }
    return node;
}

void Tree::CreateTree(Node* root, ofstream& file, string light_filed, uint hypercubo, uint channel, uint level, const Point4D &pos, const Point4D &hypercubo_pos, Point4D middle_before) {
    if (IS_ORIGINAL)
        this->size = 15;
    else if (IS_4X4X4X4)
        this->size = 4;
    else if (IS_8X8X8X8)
        this->size = 8;
    else return;

    if (root->hypercube->dim.x <= this->size || root->hypercube->dim.y <= this->size || root->hypercube->dim.u <= this->size || root->hypercube->dim.v <= this->size) {
        root->CountValues();

        if (IS_ORIGINAL){
            this->prox_pos.x = (float)pos.x;
            this->prox_pos.y = (float)pos.y;
            this->prox_pos.u = (float)pos.u;
            this->prox_pos.v = (float)pos.v;
        }
        else if (IS_4X4X4X4 || IS_8X8X8X8){
            this->prox_pos.x += (float)pos.x;
            this->prox_pos.y += (float)pos.y;
            this->prox_pos.u += (float)pos.u;
            this->prox_pos.v += (float)pos.v;
        }
        root->SetFileValues(file, light_filed, hypercubo, channel, level, prox_pos);

        return;
    }
    else{
        uint middle_x = ceil((double)root->hypercube->dim.x/2);
        uint middle_y = ceil((double)root->hypercube->dim.y/2);
        uint middle_u = ceil((double)root->hypercube->dim.u/2);
        uint middle_v = ceil((double)root->hypercube->dim.v/2);

        Hypercube *block;
        uint next_level = level + 1;

        // Block partition 0
        this->prox_pos.x = 0 + middle_before.x, this->prox_pos.y = 0 + middle_before.y, this->prox_pos.u = 0 + middle_before.u, this->prox_pos.v = 0 + middle_before.v;
        block = new Hypercube(middle_x, middle_y, middle_u, middle_v);
        get_block_partition( block->data, root->hypercube->data, 0, middle_x, 0, middle_y, 0, middle_u, 0, middle_v);
        root->child[0] = NewNode(block);
        this->CreateTree(root->child[0], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {0,0,0,0});

        // Block partition 1
        this->prox_pos.x = middle_x + middle_before.x, this->prox_pos.y = 0 + middle_before.y, this->prox_pos.u = 0 + middle_before.u, this->prox_pos.v = 0 + middle_before.v;
        block = new Hypercube(root->hypercube->dim.x - middle_x, middle_y, middle_u, middle_v);
        get_block_partition( block->data, root->hypercube->data, middle_x, root->hypercube->dim.x, 0, middle_y, 0, middle_u, 0, middle_v);
        root->child[1] = NewNode(block);
        this->CreateTree(root->child[1], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {middle_x,0,0,0});

        // Block partition 2
        this->prox_pos.x = 0 + middle_before.x, this->prox_pos.y = middle_y + middle_before.y, this->prox_pos.u = 0 + middle_before.u, this->prox_pos.v = 0 + middle_before.v;
        block = new Hypercube(middle_x, root->hypercube->dim.y - middle_y, middle_u, middle_v);
        get_block_partition( block->data, root->hypercube->data, 0, middle_x, middle_y, root->hypercube->dim.y, 0, middle_u, 0, middle_v);
        root->child[2] = NewNode(block);
        this->CreateTree(root->child[2], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {0, middle_y,0,0});

        // Block partition 3
        this->prox_pos.x = middle_x + middle_before.x, this->prox_pos.y = middle_y + middle_before.y, this->prox_pos.u = 0 + middle_before.u, this->prox_pos.v = 0 + middle_before.v;
        block = new Hypercube(root->hypercube->dim.x - middle_x, root->hypercube->dim.y - middle_y, middle_u, middle_v);
        get_block_partition( block->data, root->hypercube->data, middle_x, root->hypercube->dim.x, middle_y, root->hypercube->dim.y, 0, middle_u, 0, middle_v);
        root->child[3] = NewNode(block);
        this->CreateTree(root->child[3], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {middle_x,middle_y,0,0});

        // Block partition 4
        this->prox_pos.x = 0 + middle_before.x, this->prox_pos.y = 0 + middle_before.y, this->prox_pos.u = middle_u + middle_before.u, this->prox_pos.v = 0 + middle_before.v;
        block = new Hypercube(middle_x, middle_y, root->hypercube->dim.u - middle_u, middle_v);
        get_block_partition( block->data, root->hypercube->data, 0, middle_x, 0, middle_y, middle_u, root->hypercube->dim.u, 0, middle_v);
        root->child[4] = NewNode(block);
        this->CreateTree(root->child[4], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {0,0,middle_u,0});

        // Block partition 5
        this->prox_pos.x = middle_x +  middle_before.x, this->prox_pos.y = 0 + middle_before.y, this->prox_pos.u = middle_u + middle_before.u, this->prox_pos.v = 0 + middle_before.v;
        block = new Hypercube(root->hypercube->dim.x - middle_x, middle_y, root->hypercube->dim.u - middle_u, middle_v);
        get_block_partition( block->data, root->hypercube->data, middle_x, root->hypercube->dim.x, 0, middle_y, middle_u, root->hypercube->dim.u, 0, middle_v);
        root->child[5] = NewNode(block);
        this->CreateTree(root->child[5], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos,{middle_x,0,middle_u,0});

        // Block partition 6
        this->prox_pos.x = 0 + middle_before.x, this->prox_pos.y = middle_y + middle_before.y, this->prox_pos.u = middle_u + middle_before.u, this->prox_pos.v = 0 + middle_before.v;
        block = new Hypercube(middle_x, root->hypercube->dim.y - middle_y, root->hypercube->dim.u - middle_u, middle_v);
        get_block_partition( block->data, root->hypercube->data, 0, middle_x, middle_y, root->hypercube->dim.y, middle_u, root->hypercube->dim.u, 0, middle_v);
        root->child[6] = NewNode(block);
        this->CreateTree(root->child[6], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {0, middle_y,middle_u,0});

        // Block partition 7
        this->prox_pos.x = middle_x + middle_before.x, this->prox_pos.y = middle_y + middle_before.y, this->prox_pos.u = middle_u + middle_before.u, this->prox_pos.v = 0 + middle_before.v;
        block = new Hypercube(root->hypercube->dim.x - middle_x, root->hypercube->dim.y - middle_y, root->hypercube->dim.u - middle_u, middle_v);
        get_block_partition( block->data, root->hypercube->data, middle_x, root->hypercube->dim.x, middle_y, root->hypercube->dim.y, middle_u, root->hypercube->dim.u, 0, middle_v);
        root->child[7] = NewNode(block);
        this->CreateTree(root->child[7], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {middle_x,middle_y,middle_u,0});

        // Block partition 8
        this->prox_pos.x = 0 + middle_before.x, this->prox_pos.y = 0 + middle_before.y, this->prox_pos.u = 0 + middle_before.u, this->prox_pos.v = middle_v + middle_before.v;
        block = new Hypercube(middle_x, middle_y, middle_u, root->hypercube->dim.v - middle_v);
        get_block_partition( block->data, root->hypercube->data, 0, middle_x, 0, middle_y, 0, middle_u, middle_v, root->hypercube->dim.v);
        root->child[8] = NewNode(block);
        this->CreateTree(root->child[8], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {0,0,0,middle_v});

        // Block partition 9
        this->prox_pos.x = middle_x + middle_before.x, this->prox_pos.y = 0 + middle_before.y, this->prox_pos.u = 0 + middle_before.u, this->prox_pos.v = middle_v + middle_before.v;
        block = new Hypercube(root->hypercube->dim.x - middle_x, middle_y, middle_u, root->hypercube->dim.v - middle_v);
        get_block_partition( block->data, root->hypercube->data, middle_x, root->hypercube->dim.x, 0, middle_y, 0, middle_u, middle_v, root->hypercube->dim.v);
        root->child[9] = NewNode(block);
        this->CreateTree(root->child[9], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {middle_x,0,0,middle_v});

        // Block partition 10
        this->prox_pos.x = 0 + middle_before.x, this->prox_pos.y = middle_y + middle_before.y, this->prox_pos.u = 0 + middle_before.u, this->prox_pos.v = middle_v + middle_before.v;
        block = new Hypercube(middle_x, root->hypercube->dim.y - middle_y, middle_u, root->hypercube->dim.v - middle_v);
        get_block_partition( block->data, root->hypercube->data, 0, middle_x, middle_y, root->hypercube->dim.y, 0, middle_u, middle_v, root->hypercube->dim.v);
        root->child[10] = NewNode(block);
        this->CreateTree(root->child[10], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos,{0, middle_y,0,middle_v});

        // Block partition 11
        this->prox_pos.x = middle_x + middle_before.x, this->prox_pos.y = middle_y + middle_before.y, this->prox_pos.u = 0 + middle_before.u, this->prox_pos.v = middle_v + middle_before.v;
        block = new Hypercube(root->hypercube->dim.x - middle_x, root->hypercube->dim.y - middle_y, middle_u, root->hypercube->dim.v - middle_v);
        get_block_partition( block->data, root->hypercube->data, middle_x, root->hypercube->dim.x, middle_y, root->hypercube->dim.y, 0, middle_u, middle_v, root->hypercube->dim.v);
        root->child[11] = NewNode(block);
        this->CreateTree(root->child[11], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {middle_x,middle_y,0,middle_v});

        // Block partition 12
        this->prox_pos.x = 0 + middle_before.x, this->prox_pos.y = 0 + middle_before.y, this->prox_pos.u = middle_u + middle_before.u, this->prox_pos.v = middle_v + middle_before.v;
        block = new Hypercube(middle_x, middle_y, root->hypercube->dim.u - middle_u, root->hypercube->dim.v - middle_v);
        get_block_partition( block->data, root->hypercube->data, 0, middle_x, 0, middle_y, middle_u, root->hypercube->dim.u, middle_v, root->hypercube->dim.v);
        root->child[12] = NewNode(block);
        this->CreateTree(root->child[12], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {0,0,middle_u,middle_v});

        // Block partition 13
        this->prox_pos.x = middle_x + middle_before.x, this->prox_pos.y = 0 + middle_before.y, this->prox_pos.u = middle_u + middle_before.u, this->prox_pos.v = middle_v + middle_before.v;
        block = new Hypercube(root->hypercube->dim.x - middle_x, middle_y, root->hypercube->dim.u - middle_u, root->hypercube->dim.v - middle_v);
        get_block_partition( block->data, root->hypercube->data, middle_x, root->hypercube->dim.x, 0, middle_y, middle_u, root->hypercube->dim.u, middle_v, root->hypercube->dim.v);
        root->child[13] = NewNode(block);
        this->CreateTree(root->child[13], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {middle_x,0,middle_u,middle_v});

        // Block partition 14
        this->prox_pos.x = 0 + middle_before.x, this->prox_pos.y = middle_y + middle_before.y, this->prox_pos.u = middle_u + middle_before.u, this->prox_pos.v = middle_v + middle_before.v;
        block = new Hypercube(middle_x, root->hypercube->dim.y - middle_y, root->hypercube->dim.u - middle_u, root->hypercube->dim.v - middle_v);
        get_block_partition( block->data, root->hypercube->data, 0, middle_x, middle_y, root->hypercube->dim.y, middle_u, root->hypercube->dim.u, middle_v, root->hypercube->dim.v);
        root->child[14] = NewNode(block);
        this->CreateTree(root->child[14], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {0, middle_y,middle_u,middle_v});

        // Block partition 15
        this->prox_pos.x = middle_x + middle_before.x, this->prox_pos.y = middle_y + middle_before.y, this->prox_pos.u = middle_u + middle_before.u, this->prox_pos.v = middle_v + middle_before.v;
        block = new Hypercube(root->hypercube->dim.x - middle_x, root->hypercube->dim.y - middle_y, root->hypercube->dim.u - middle_u, root->hypercube->dim.v - middle_v);
        get_block_partition( block->data, root->hypercube->data, middle_x, root->hypercube->dim.x, middle_y, root->hypercube->dim.y, middle_u, root->hypercube->dim.u, middle_v, root->hypercube->dim.v);
        root->child[15] = NewNode(block);
        this->CreateTree(root->child[15], file, light_filed, hypercubo, channel, next_level, pos, hypercubo_pos, {middle_x,middle_y,middle_u,middle_v});
      }
}

void Tree::get_block_partition(int ****block, int ****data, int start_x, int end_x, int start_y, int end_y, int start_u, int end_u, int start_v, int end_v) {
    for (int block_it_v = start_v, pos_v = 0; block_it_v < end_v; ++block_it_v, ++pos_v) {
        for (int block_it_u = start_u, pos_u = 0; block_it_u < end_u; ++block_it_u, ++pos_u) {
            for (int block_it_y = start_y, pos_y = 0; block_it_y < end_y; ++block_it_y, ++pos_y) {
                for (int block_it_x = start_x, pos_x = 0; block_it_x < end_x; ++block_it_x, ++pos_x) {
                    block[pos_x][pos_y][pos_u][pos_v] = data[block_it_x][block_it_y][block_it_u][block_it_v]; //SEGMENTATION HERE
                }
            }
        }
    }
}

void Tree::_deleteTree(Node* node)
{
    if (node == nullptr) return;

    for (int i = 0; i < node->hypercube->dim.x; ++i) {
        for (int k = 0; k < node->hypercube->dim.y; ++k) {
            for (int l = 0; l < node->hypercube->dim.u; ++l) {
                delete [] node->hypercube->data[i][k][l];
            }
            delete [] node->hypercube->data[i][k];
        }
        delete [] node->hypercube->data[i];
    }
    delete [] node->hypercube->data;
    
    for (int i = 0; i < node->child.size(); ++i) {
        _deleteTree(node->child[i]);
    }
    delete node;
}

/* Deletes a tree and sets the root as NULL */
void Tree::DeleteTree(Node** node_ref)
{
    _deleteTree(*node_ref);
    *node_ref = nullptr;
}