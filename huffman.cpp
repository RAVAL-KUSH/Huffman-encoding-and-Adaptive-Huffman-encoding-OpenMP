#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <omp.h>
#include <cmath>

#define MAX_TREE_HT 50

//clocks
clock_t encode_start, encode_end, decode_start, decode_end, buildHuffTree_start, buildHuffTree_end, freq_start, freq_end, compress_start, compress_end;

inline double wallTime(const clock_t& s, const clock_t& e)
{
    return (double)(e - s)/(double)CLOCKS_PER_SEC;
}

struct MinHNode {
    unsigned freq;
    uint16_t item;
    struct MinHNode *left, *right;
};

struct MinH {
    unsigned size;
    unsigned capacity;
    struct MinHNode **array;
};

// Creating Huffman tree node
struct MinHNode *newNode(uint16_t item, unsigned freq) 
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

struct MinH *createAndBuildMinHeap(const std::vector<uint16_t>& item,const std::vector<unsigned>& freq) 
{
    int size = freq.size();
    struct MinH *minHeap = createMinH(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(item[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    return minHeap;
}

struct MinHNode *buildHfTree(const std::vector<uint16_t>& item,const std::vector<unsigned>& freq) 
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

void printHCodes(struct MinHNode *root, int arr[], int top, std::map<uint16_t,std::string>& huffCodes) 
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

void huffmanEncoder(const std::vector<uint16_t>& message, std::string& encodedMessage, MinHNode** root, double& compressionRatio)
{
    int length = message.size();
    std::map<uint16_t,long int> freqMap;
    freq_start = clock();
    for(auto& c : message)
    {
        freqMap[c]++;
    }
    std::vector<unsigned> freq(freqMap.size());
    std::vector<uint16_t> item(freqMap.size());
    int i = 0 ;
    for(auto& it : freqMap)
    {
        item[i]=it.first;
        freq[i]=it.second;
        i++;
    }
    freq_end = clock();
    buildHuffTree_start = clock();
    *root = buildHfTree(item, freq);
    buildHuffTree_end = clock();
    int arr[MAX_TREE_HT], top = 0;
    std::map<uint16_t,std::string> huffCodes;
    //cout << "Huffman Codes: \n";
    printHCodes(*root, arr, top, huffCodes);
    std::vector<uint16_t> temp;
    compress_start = clock();
    for(auto& i : message)
        encodedMessage += huffCodes[i];
    compress_end = clock();
    double up=0,down=0;
    for(auto& i : huffCodes)
    {
        up += 8*freqMap[i.first];
        down += i.second.size() * freqMap[i.first];
    }
    compressionRatio = up/down;
}

std::vector<uint16_t> huffmanDecoder(const std::string& message, MinHNode* root)
{
    std::vector<uint16_t> decoded;
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
    int size;
    std::vector<uint16_t> dataArr;
        std::string filename;
    int ch;
    std::cout <<  
        "\nChoose dataset : \n"
        "1. bonsai CT scan (256x256x256) \n"
        "2. aneurism CT scan (256x256x256) \n"
        "3. head aneurism CT scan (512x512x512) \n"
        "4. pancreas CT scan (240x512x512) \n";
        //"5. Magnetic reconnection simulatuion (512x512x512) \n";
    std::cin >> ch;
    switch(ch)
    {   
        case 1:
            filename = "bonsai_256x256x256_uint8.raw";
            size = 1;
            break;
        case 2:
            filename = "aneurism_256x256x256_uint8.raw";
            size = 1;
            break;
        case 3:
            filename = "vertebra_512x512x512_uint16.raw";
            size = 2;
            break;
        case 4:
            filename = "pancreas_240x512x512_int16.raw";
            size = 2;
            break;

    }

    std::ifstream dataBuff(filename, std::ios::binary | std::ios::in);
    if(!dataBuff.is_open())
    {
        std::cout << "Error opening file " << std::endl;
        return;
    }   

    char temp[size];
    int ind = 0;
    while(dataBuff.read(temp, sizeof(temp)))
    {
        dataArr.push_back((uint16_t)temp[0]);
    }


    std::string encodedMessage;
    MinHNode* root;
    double compressionRatio;
    encode_start = clock();
    huffmanEncoder(dataArr, encodedMessage, &root, compressionRatio);
    encode_end = clock();
    decode_start = clock();
    std::vector<uint16_t> decodedMess = huffmanDecoder(encodedMessage, root);
    decode_end = clock();
    double total_Walltime = (decode_end - encode_start)/(double)CLOCKS_PER_SEC;
    double encode_wallTime = (encode_end- encode_start)/(double)CLOCKS_PER_SEC;
    double decode_wallTime = (decode_end- decode_start)/(double)CLOCKS_PER_SEC;
    double buildHuff_wallTime = (buildHuffTree_end - buildHuffTree_start)/(double)CLOCKS_PER_SEC;
    double freqMap_wallTime = (freq_end - freq_start)/(double)CLOCKS_PER_SEC;
    double compression_wallTime = (compress_end - compress_start)/(double)CLOCKS_PER_SEC;

    printf("Message Size (array length): %ld \n", dataArr.size());
    printf("Message Size (bits): %ld \n", dataArr.size()*size*8.0);
    printf("Huffman Encoded Message Size: %ld \n", encodedMessage.size());
    printf("Decoded Message Size: %ld \n", decodedMess.size());
    // printf("Total runtime: %.6f \n", total_Walltime);
    // printf("    +-Encoding Runtime: %.6f \n", encode_wallTime);
    // printf("    |   +-Frequency Table: %.6f \n", freqMap_wallTime);
    // printf("    |   +-Build Huffman Tree: %.6f \n", buildHuff_wallTime);
    // printf("    |   +-Compression: %.6f \n", compression_wallTime);
    // printf("    +-Decoding Runtime: %.6f \n", decode_wallTime);
    // printf("Compression Ratio: %.6f \n", compressionRatio);

    printf("1, %.6f, %0.6f, %0.6f, %0.6f, %0.6f, %.6f \n", total_Walltime, freqMap_wallTime, compression_wallTime, buildHuff_wallTime, decode_wallTime, compressionRatio);

}

void parallelImp()
{
    int size;
    std::vector<uint16_t> dataArr;
    std::string filename;
    int ch;
    std::cout <<  
        "\nChoose dataset : \n"
        "1. bonsai CT scan (256x256x256) \n"
        "2. aneurism CT scan (256x256x256) \n"
        "3. head aneurism CT scan (512x512x512) \n"
        "4. pancreas CT scan (240x512x512) \n";
        //"5. Magnetic reconnection simulatuion (512x512x512) \n";
    std::cin >> ch;
    switch(ch)
    {   
        case 1:
            filename = "bonsai_256x256x256_uint8.raw";
            size = 1;
            break;
        case 2:
            filename = "aneurism_256x256x256_uint8.raw";
            size = 1;
            break;
        case 3:
            filename = "vertebra_512x512x512_uint16.raw";
            size = 2;
            break;
        case 4:
            filename = "pancreas_240x512x512_int16.raw";
            size = 2;
            break;

    }

    std::ifstream dataBuff(filename, std::ios::binary | std::ios::in);
    if(!dataBuff.is_open())
    {
        std::cout << "Error opening file " << std::endl;
        return;
    }   

    char temp[size];
    int ind = 0;
    while(dataBuff.read(temp, sizeof(temp)))
    {
        dataArr.push_back((uint16_t)temp[0]);
    }

    int p;
    clock_t start, end;
    // std::cout << "No of procs: ";
    // std::cin >> p;
    for(int powr = 1 ; powr <= 4 ; powr++)
    {
        p = std::pow(2, powr);
    
        //divide workload
        std::vector<MinHNode*> root(p);
        std::vector<std::vector<uint16_t>> dataChunkArr(p);
        std::vector<std::string> encodedMessage(p);

        for(int i = 0 ; i < p ; i++)
            root[i] = new MinHNode();
        int chunkSize = dataArr.size()/p;
        for(uint64_t i = 0 ; i < dataArr.size() ; i++)
            dataChunkArr[i/chunkSize].push_back(dataArr[i]);


        //divide workload

        double compressionRatio;

        double freqTime;
        double buildHuffWalltime;
        double compressWalltime;
        double encodeWalltime;
        double decodeWalltime;
        
        std::vector<uint16_t> decodedMess(p);
        omp_set_num_threads(p);
        start = clock();
        #pragma omp parallel 
        {
            int n = omp_get_thread_num();
            double l_compressionRatio;
            encode_start = clock();
            huffmanEncoder(dataChunkArr[n], encodedMessage[n], &root[n], l_compressionRatio);
            encode_end = clock();
            decode_start = clock();
            std::vector<uint16_t> temp = huffmanDecoder(encodedMessage[n], root[n]);
            decode_end = clock();
            decodedMess.insert(decodedMess.end(), temp.begin(), temp.end());
            if(n==0)
            {
                freqTime = wallTime(freq_start, freq_end);
                buildHuffWalltime = wallTime(buildHuffTree_start, buildHuffTree_end);
                compressWalltime = wallTime(compress_start, compress_end);
                compressionRatio = l_compressionRatio;
                encodeWalltime = wallTime(encode_start, encode_end);
                decodeWalltime = wallTime(decode_start, decode_end);
            }
        }
        end = clock();
        double totalWalltime = wallTime(start, end);
        printf("%d, %0.6f, %0.6f, %0.6f, %0.6f, %0.6f, %.6f \n", p, totalWalltime, freqTime, compressWalltime, buildHuffWalltime, decodeWalltime, compressionRatio);
        for(int i = 0 ; i < p ; i++)    
        {
            DeleteTree(root[i]);
        }
    }
    
}

enum Code{
    serial=1,
    parallel
};

//serial implementation
int main()
{
    int ch;
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