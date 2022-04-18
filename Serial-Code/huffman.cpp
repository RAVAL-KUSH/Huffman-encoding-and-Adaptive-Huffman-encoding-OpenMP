#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>

#define MAX_TREE_HT 50

struct MinHNode {
    unsigned freq;
    uint8_t item;
    struct MinHNode *left, *right;
};

struct MinH {
    unsigned size;
    unsigned capacity;
    struct MinHNode **array;
};

// Creating Huffman tree node
struct MinHNode *newNode(uint8_t item, unsigned freq) 
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
        std::cout << arr[i];
    std::cout << "\n";
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

struct MinH *createAndBuildMinHeap(const std::vector<uint8_t>& item,const std::vector<unsigned>& freq) 
{
    int size = freq.size();
    struct MinH *minHeap = createMinH(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(item[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    return minHeap;
}

struct MinHNode *buildHfTree(const std::vector<uint8_t>& item,const std::vector<unsigned>& freq) 
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

void printHCodes(struct MinHNode *root, int arr[], int top, std::map<uint8_t,std::string>& huffCodes) 
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
        // cout << root->item << "  | ";
        // printArray(arr, top);
        std::string tempcode;
        for(int i = 0 ; i < top; i++ )
            tempcode += std::to_string(arr[i]);
        huffCodes[root->item]+=tempcode;
        
    }
}

bool cmp(std::pair<char,unsigned>& a, std::pair<char,unsigned>& b)
{
    return a.second < b.second;
}

void huffmanEncoder(const std::vector<uint8_t>& message, std::string& encodedMessage, MinHNode** root)
{
    int length = message.size();
    std::map<uint8_t,int> freqMap;
    for(auto& c : message)
        freqMap[c]++;
    std::vector<unsigned> freq(freqMap.size());
    std::vector<uint8_t>item(freqMap.size());
    int i = 0;
    for(auto& it : freqMap)
    {
        item[i]=it.first;
        freq[i]=it.second;
        i++;
    }
    *root = buildHfTree(item, freq);
    int arr[MAX_TREE_HT], top = 0;
    std::map<uint8_t,std::string> huffCodes;
    //cout << "Huffman Codes: \n";
    printHCodes(*root, arr, top, huffCodes);
    std::vector<uint8_t> temp;
    for(auto& i : message)
            encodedMessage += huffCodes[i];
    
}

std::vector<uint8_t> huffmanDecoder(const std::string& message, MinHNode* root)
{
    std::vector<uint8_t> decoded;
    MinHNode* curr = root;
    for(auto& i : message)
    {
        if(i=='0')
            curr = curr->left;
        else
            curr = curr->right;
        if(isLeaf(curr))
        {
            decoded.push_back(curr->item);
            curr = root;
        }
    }
    return decoded;
}

void DeleteTree(MinHNode *curr)
{
    if(curr == nullptr) return;
    DeleteTree(curr->left);
    DeleteTree(curr->right);
    delete curr;
}

void serialImp()
{
    std::cout 
        <<  "Choose dataset : \n" 
            "1. bonsai CT scan (256x256x256) \n"
            "2. anu"
    clock_t start, end;
    std::ifstream dataBuff("bonsai_256x256x256_uint8.raw", std::ios::binary | std::ios::in);
    if(!dataBuff.is_open())
    {
        std::cout << "Error opening file " << std::endl;
        return;
    }
    //uint8_t dataArr[256][256][256];
    std::vector<uint8_t> dataArr;
    char temp[sizeof(uint8_t)];
    int ind = 0;
    while(dataBuff.read(temp, sizeof(temp)))
    {
        dataArr.push_back((uint8_t)atoi(temp));
    }
    std::string encodedMessage;
    MinHNode* root;


    huffmanEncoder(dataArr, encodedMessage, &root);
    std::vector<uint8_t> decodedMess = huffmanDecoder(encodedMessage, root);

    std::cout << "Message Size: " << dataArr.size() << std::endl;
    std::cout << "8-bit Message Size:" << dataArr.size()*8 << std::endl;
    std::cout << "Huffman Encoded Message Size:" << encodedMessage.size() << std::endl;
    std::cout << "Decoded Message Size: " << decodedMess.size() << std::endl;
}

void parallelImp()
{

}

enum Code{
    serial=1,
    parallel
};

//serial implementation
int main()
{
    uint8_t ch;
    std::cout << "Serial[1] or Parallel[2] ? ";
    std::cin >> ch ;
    switch (ch)
    {
    case Code::serial:
        serialImp();
        break;
    
    case Code::parallel:
        parallelImp();
        break;

    default:
        break;
    }
    
    return 0;
}