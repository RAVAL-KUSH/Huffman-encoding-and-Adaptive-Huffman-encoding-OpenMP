#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

namespace serial
{
    #define MAX_TREE_HT 50

    struct MinHNode {
        unsigned freq;
        char item;
        struct MinHNode *left, *right;
    };

    struct MinH {
        unsigned size;
        unsigned capacity;
        struct MinHNode **array;
    };

    // Creating Huffman tree node
    struct MinHNode *newNode(char item, unsigned freq) 
    {
        struct MinHNode *temp = (struct MinHNode *)malloc(sizeof(struct MinHNode));

        temp->left = temp->right = NULL;
        temp->item = item;
        temp->freq = freq;

        return temp;
    }

    // Create min heap using given capacity
    struct MinH *createMinH(unsigned capacity) 
    {
        struct MinH *minHeap = (struct MinH *)malloc(sizeof(struct MinH));
        minHeap->size = 0;
        minHeap->capacity = capacity;
        minHeap->array = (struct MinHNode **)malloc(minHeap->capacity * sizeof(struct MinHNode *));
        return minHeap;
    }

    // Print the array
    void printArray(int arr[], int n) 
    {
        int i;
        for (i = 0; i < n; ++i)
            cout << arr[i];

        cout << "\n";
    }

    // Swap function
    void swapMinHNode(struct MinHNode **a, struct MinHNode **b) 
    {
        struct MinHNode *t = *a;
        *a = *b;
        *b = t;
    }

    // Heapify
    void minHeapify(struct MinH *minHeap, int idx) 
    {
        int smallest = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;

        if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
            smallest = left;

        if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
            smallest = right;

        if (smallest != idx) 
        {
            swapMinHNode(&minHeap->array[smallest], &minHeap->array[idx]);
            minHeapify(minHeap, smallest);
        }
    }

    // Check if size if 1
    int checkSizeOne(struct MinH *minHeap) 
    {
        return (minHeap->size == 1);
    }

    // Extract the min
    struct MinHNode *extractMin(struct MinH *minHeap) 
    {
        struct MinHNode *temp = minHeap->array[0];
        minHeap->array[0] = minHeap->array[minHeap->size - 1];

        --minHeap->size;
        minHeapify(minHeap, 0);

        return temp;
    }

    // Insertion
    void insertMinHeap(struct MinH *minHeap, struct MinHNode *minHeapNode) 
    {
        ++minHeap->size;
        int i = minHeap->size - 1;

        while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) 
        {
            minHeap->array[i] = minHeap->array[(i - 1) / 2];
            i = (i - 1) / 2;
        }

        minHeap->array[i] = minHeapNode;
    }

    // BUild min heap
    void buildMinHeap(struct MinH *minHeap) 
    {
        int n = minHeap->size - 1;
        int i;

        for (i = (n - 1) / 2; i >= 0; --i)
            minHeapify(minHeap, i);
    }

    int isLeaf(struct MinHNode *root) 
    {
        return !(root->left) && !(root->right);
    }

    struct MinH *createAndBuildMinHeap(const vector<char>& item,const vector<unsigned>& freq) 
    {
        int size = freq.size();
        struct MinH *minHeap = createMinH(size);

        for (int i = 0; i < size; ++i)
            minHeap->array[i] = newNode(item[i], freq[i]);

        minHeap->size = size;
        buildMinHeap(minHeap);

        return minHeap;
    }

    struct MinHNode *buildHfTree(const vector<char>& item,const vector<unsigned>& freq) 
    {

        struct MinHNode *left, *right, *top;
        struct MinH *minHeap = createAndBuildMinHeap(item, freq);

        while (!checkSizeOne(minHeap)) 
        {
            left = extractMin(minHeap);
            right = extractMin(minHeap);

            top = newNode('$', left->freq + right->freq);

            top->left = left;
            top->right = right;

            insertMinHeap(minHeap, top);
        }
        return extractMin(minHeap);
    }

    void printHCodes(struct MinHNode *root, int arr[], int top, map<char,string>& huffCodes) 
    {
        if (root->left) 
        {
            arr[top] = 0;
            printHCodes(root->left, arr, top + 1, huffCodes);
        }

        if (root->right) 
        {
            arr[top] = 1;
            printHCodes(root->right, arr, top + 1, huffCodes);
        }
        if (isLeaf(root)) 
        {
            cout << root->item << "  | ";
            printArray(arr, top);
            string tempcode;
            for(int i = 0 ; i < top; i++ )
                tempcode += to_string(arr[i]);
            huffCodes[root->item]+=tempcode;
            
        }
    }

    bool cmp(pair<char,unsigned>& a, pair<char,unsigned>& b)
    {
        return a.second < b.second;
    }

    void huffmanEncoder(const string& message, string& encodedMessage, MinHNode** root)
    {
        int length = message.size();
        map<char,int> freqMap;
        for(auto& c : message)
            freqMap[c]++;
        vector<unsigned> freq(freqMap.size());
        vector<char>item(freqMap.size());
        int i = 0;
        for(auto& it : freqMap)
        {
            item[i]=it.first;
            freq[i]=it.second;
            i++;
        }
        *root = buildHfTree(item, freq);
        int arr[MAX_TREE_HT], top = 0;
        map<char,string> huffCodes;
        cout << "Huffman Codes: \n";
        printHCodes(*root, arr, top, huffCodes);
        for(auto& i : message)
            encodedMessage += huffCodes[i];
        
    }

    string huffmanDecoder(const string& message, MinHNode* root)
    {
        string decoded;
        MinHNode* curr = root;
        for(auto& i : message)
        {
            if(i=='0')
                curr = curr->left;
            else
                curr = curr->right;
            if(isLeaf(curr))
            {
                decoded += curr->item;
                curr = root;
            }
        }
        return decoded+'\0';
    }

    void DeleteTree(MinHNode *curr)
    {
        if(curr == nullptr) return;
        DeleteTree(curr->left);
        DeleteTree(curr->right);
        delete curr;
    }
}


int main()
{
    string message = "lorem ipsum ipsum";
    string encodedMessage;
    map<char,string> huffCodes;
    serial::MinHNode* root;
    serial::huffmanEncoder(message, encodedMessage, &root);
    cout << "Encoded Message: " << encodedMessage << endl;
    string decodedMessage = serial::huffmanDecoder(encodedMessage,root);
    cout << "Decoded Message: " << decodedMessage << endl;
    serial::DeleteTree(root);
    return 0;
}