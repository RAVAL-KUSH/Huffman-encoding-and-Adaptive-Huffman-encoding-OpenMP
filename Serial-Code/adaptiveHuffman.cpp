#include <iostream>
#include <map>
using namespace std;



namespace serial
{

    struct node
    {
        char value;
        int weight;
        int order;
        bool isNYT;
        node *left, *right, *parent;
        node(char value, int weight, int order, node* left, node* right, node* parent, bool isNYT = false)
        :value(value), weight(weight), order(order), left(left), right(right), parent(parent), isNYT(isNYT) {};
    };

    void updateHuffmanTree(const char& symbol)
    {
        node* newLeaf = nullptr;
        node* symbolNode = GetSymbolNode(symbol, root);

        if(symbolNode == nullptr)
        {
            NYTnode->isNYT = false;
            int currentMinOrder = NYTnode->order;
            NYTnode->left = new node(-1, 0, currentMinOrder - 2, nullptr, nullptr, NYTnode, true);
		    NYTnode->right = new node(symbol, 0, currentMinOrder - 1, nullptr, nullptr, NYTnode, false);
            symbolNode = NYTnode->right;
            NYTnode = NYTnode->left;
        }
        SlideAndIncrement(symbolNode);
    }

    void SlideAndIncrement(node* curr)
    {
        if(curr == nullptr) return;
        node *blockLeader = curr;
        FindBlockLeader(root, blockLeader);
        if(blockLeader != curr) 
            SwapNodes(blockLeader, curr);
        ++curr->weight;
        SlideAndIncrement(curr->parent);
    }

    void FindBlockLeader(node* curr, node* &currMax)
    {
        if(curr==nullptr || currMax == root) return;
        if (curr->weight == currMax->weight && curr != currMax->parent  && curr->order > currMax->order)
	    {
		    currMax = curr;
	    }
        FindBlockLeader(curr->left, currMax);
        FindBlockLeader(curr->right, currMax);
    }

    void SwapNodes(node* a, node* b)
    {
        if (a->parent == nullptr || b->parent == nullptr) return;
    
        if (a->parent == b || b->parent ==a) return;

        node *&aRef =a->parent->left ==a ?a->parent->left :a->parent->right;
        node *&bRef = b->parent->left == b ? b->parent->left : b->parent->right;

        std::swap(aRef, bRef);
        std::swap(aRef->parent, bRef->parent);
        std::swap(aRef->order, bRef->order);

    }

    node* GetSymbolNode(const char& symbol, node* curr) 
    {
        if(curr == nullptr || curr->value == symbol)
        {
            return curr;
        }

        node* leftResult = GetSymbolNode(symbol, curr->left);
        return leftResult == nullptr ? GetSymbolNode(symbol, curr->right) : leftResult;
    }

    string Encode(const char& symbol)
    {
        node* symbolNode = GetSymbolNode(symbol,root);
        if(symbolNode != nullptr)
        {
               
        }
    }

    string GetPathToSymbol(node* curr, node* result, string currPath)
    {
        if(curr == result)
            return currPath;
        if(curr == nullptr)
            return "";
        
        string left = GetPathToSymbol(curr->left, result, currPath+"0");
        return left != "" ? left : GetPathToSymbol(curr->left, result, currPath+"1");
    }

    string huffmanEncoder(const string& message)
    {
        for(auto& i : message)
            encode(i)
        string encodedMessage;
        for(auto& i : message)
            
    }

    string huffmanDecoder(const string& encodedmessage)
    {
        string decoded;
        return decoded;
    }

    void DeleteTree(node* curr)
    {
        if(curr == nullptr) return;
        DeleteTree(curr->left);
        DeleteTree(curr->right);
        delete curr;
    }
}


serial::node* root = new serial::node(-1, 0, 512, nullptr, nullptr, nullptr, true);
serial::node* NYTnode = root;

int main()
{
    string message = "lorem ipsum ipsum";
    string encodedMessage;
    map<char,string> huffCodes;
    for(auto& i : message)
        serial::updateHuffmanTree(i);
    //serial::huffmanEncoder(message, encodedMessage, &root);
    cout << "Encoded Message: " << encodedMessage << endl;
    string decodedMessage = serial::huffmanDecoder(encodedMessage);
    cout << "Decoded Message: " << decodedMessage << endl;

    return 0;
}