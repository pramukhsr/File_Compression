//
// STARTER CODE: util.h
//
// This function uses the HuffmanNode algorithm to compress text files. It is
// a great way to decrease file size based off of the most populous characters.
//
#include "hashmap.h"
#include "bitstream.h"
#include "priorityqueue.h"
#pragma once
#include <vector>
#include <string>

typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};
// helper function to help clear the tree
void _freeTree(HuffmanNode* node) {
    if (node == nullptr) {
        return;
    }
    _freeTree(node->zero);
    _freeTree(node->one);
    delete node;
}
//
// This function will clear the encoding tree.
//
void freeTree(HuffmanNode* node) {
    _freeTree(node);
    node = nullptr;
}

//
// This function builds an encoding map based of the file. All unique characters
// are put into the map and duplicates increase the count.
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {
    if (!isFile) {
        for (auto x : filename) {
            if (map.containsKey(x)) {
                int y = map.get(x) + 1;
                map.put(x, y);
            } else {
                map.put(x, 1);
            }
        }
    }  else {
        char x;
        ifstream infile(filename);
        while (!infile.eof()) {
            infile.get(x);
            if (!infile.fail()) {
                if ( map.containsKey(x) ) {
                    int y = map.get(x) + 1;
                    map.put(x, y);
                } else {
                    map.put(x, 1);
                }
            }
        }
    }
    map.put(PSEUDO_EOF, 1);
}

//
// This function builds a tree based off the frequency map. You build the tree
// based off of the first two nodes in the map and then add their frequencies
// together.
//
HuffmanNode* buildEncodingTree(hashmapF &map) {
    priorityqueue<HuffmanNode*> pQueue;
    vector<int> treeVals = map.keys();
    for (size_t i = 0; i < treeVals.size(); i++) {
        HuffmanNode* temp = new HuffmanNode;
        temp->one = nullptr;
        temp->zero = nullptr;
        temp->character = treeVals[i];
        temp->count = map.get(treeVals[i]);
        pQueue.enqueue(temp, temp->count);
    }
    while (pQueue.Size() != 1) {
        HuffmanNode* temp = new HuffmanNode;
        temp->zero = pQueue.dequeue();
        temp->one = pQueue.dequeue();
        temp->count = temp->zero->count + temp->one->count;
        temp->character = NOT_A_CHAR;
        pQueue.enqueue(temp, temp->count);
    }
    return pQueue.peek();
}

//
// *Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode* node, hashmapE &encodingMap, string str,
                       HuffmanNode* prev) {
    if (node == nullptr) {
        return;
    }
    if (node->zero == nullptr && node->one == nullptr) {
        encodingMap.emplace(node->character, str);
    }
    if (node->zero != nullptr) {
        _buildEncodingMap(node->zero, encodingMap, str+"0", prev);
    }
    if (node->one != nullptr) {
        _buildEncodingMap(node->one, encodingMap, str+"1", prev);
    }
}

//
// *This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode* tree) {
    hashmapE encodingMap;
    if (tree != nullptr) {
        string str = "";
        HuffmanNode* prev = nullptr;
        _buildEncodingMap(tree, encodingMap, str, prev);
    }
    return encodingMap;
}
//
// This function will encode the map. Using both the input and output streams,
// we convert the characters to bits and return the string of 0's and 1's and
// return the size of the string too.
//
string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output,
              int &size, bool makeFile) {
    char x;
    string y;
    size = 0;
    if (makeFile == true) {
        while (input.get(x)) {
        y += encodingMap[x];
        }
        y += encodingMap[PSEUDO_EOF];
        for (char z : y) {
            output.writeBit(z-'0');
        }
    } else if (makeFile == false) {
        while (input.get(x)) {
        y += encodingMap[x];
        }
        y += encodingMap[PSEUDO_EOF];
    }
    size = y.size();
    return y;
}
//
// This function does the opposite of encode. Taking in an input stream of 0's
// and 1's. we can use readBit to read through the tree and return the original
// string.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    char x;
    int y;
    string str = "";
    HuffmanNode* temp = encodingTree;

    while (!input.eof() && !input.fail()) {
        y = input.readBit();
        if (y == -1) {
            break;
        }
        if (temp->zero == nullptr && temp->one == nullptr) {
            if (temp->character == PSEUDO_EOF || temp->character == EOF) {
                break;
            }
            x = temp->character;
            output << x;
            str += x;
            temp = encodingTree;
        }
        if (y == 0) {
            temp = temp->zero;
        } else {
            temp = temp->one;
        }
    }
    return str;
}

//
// This function takes all the previous functions up until decode to return
// the encoded string of the .txt file. You have to build the frequency map,
// build the encoding tree, and build the encoding map. Then finally, enocde
// the map and return the string.
//
string compress(string filename) {
    bool isFile = true;
    hashmapF cMap;
    buildFrequencyMap(filename, isFile, cMap);
    HuffmanNode* temp = buildEncodingTree(cMap);
    hashmapE newMap = buildEncodingMap(temp);

    string fn = (isFile) ? filename : ("file_" + filename + ".txt");
    ofbitstream output(filename + ".huf");
    ifstream input(filename);
    stringstream ss;
    ss << cMap;
    output << cMap;
    int size = 0;
    string codeStr = encode(input, newMap, output, size, true);
    freeTree(temp);
    return codeStr;
}

//
// This function is the final step of decompression. Given a compressed .huf
// file we will decompress and add the actual .txt file to the new _unc.txt file
//
string decompress(string filename) {
    size_t pos = filename.find(".huf");
    if ((int)pos >= 0) {
        filename = filename.substr(0, pos);
    }
    pos = filename.find(".");
    string ext = filename.substr(pos, filename.length() - pos);
    filename = filename.substr(0, pos);
    ifbitstream input(filename + ext + ".huf");
    ofstream output(filename + "_unc" + ext);

    hashmapF dump;
    input >> dump;

    HuffmanNode* temp = buildEncodingTree(dump);

    string decodeStr  = decode(input, temp, output);
    freeTree(temp);
    return decodeStr;
}
