//
// Created by kunal on 19/11/17.
//

#include <bits/stdc++.h>
#include <omp.h>

using namespace std;


#define N 1'00'000
#define TABLE_SIZE 192

template<class T>
bool contains(const std::vector<T> &vec, const T &value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

vector<int> keys;

void generateData() {
    vector<int> keys;
    int num;
    int maxValue = INT_MAX;
    fstream input_data("/home/kunal/Documents/GITPRO/Parallel-Hashing-Implementation/test/input1", ios::out);
    if (!input_data) {
        cout << "IO Error";
        return;
    }
    for (int i = 0; i < N; ++i) {
        num = rand() % (maxValue);
        while (contains(keys, num))
            num = rand() % (maxValue);
        keys.push_back(num);
    }
    for (auto x : keys) {
        input_data << x << ' ';
        cout << x << ' ';
    }
    input_data.close();
}

void getData(string filename) {
    ifstream input_data(filename, ios::in);
    if (!input_data) {
        cout << "IO Error";
        return;
    }
    int num;
    for (int i = 0; i < N; ++i) {
        input_data >> num;
//        cout<<num<<' ';
        keys.push_back(num);
    }
    input_data.close();
}

int getBucketNumber(int key, int noOfBuckets) {
    return key % noOfBuckets;
}

class HashFunctions {
public:

    int c0, c1;

    HashFunctions(int c0, int c1) {
        this->c0 = c0;
        this->c1 = c1;
    }

    int g(int key) {
        return ((c0 + c1 * key) % 1900813) % TABLE_SIZE;
    }
};

int main() {
//    generateData();
    getData("/home/kunal/Documents/GITPRO/Parallel-Hashing-Implementation/test/input1");
//    for (auto x: keys) {
//        cout << x << ' ';
//    }

    vector<vector<vector<pair<int, bool> > > > hashTable;
    hashTable.resize(noOfBuckets);
    for (int i = 0; i < noOfBuckets; ++i) {
        hashTable[i].resize(3);
        for (int j = 0; j < 3; ++j) {
            hashTable[i][j].resize(TABLE_SIZE, {0, false});
        }
    }
    int noOfBuckets = ceil(N / 409);
    vector<int> bucket_size(noOfBuckets, 0);
    for (int i = 0; i < N; ++i) {
        bucket_size[getBucketNumber(keys[i], noOfBuckets)]++;
    }
    if (*max_element(bucket_size.begin(), bucket_size.end()) > 512) {
        cout << "h() failed!" << endl;
        return 1;
    }
    HashFunctions f[3];
    int rand_number = rand();
    int XOR_NUM[3][2] = {{69,     696},
                         {6969,   69696},
                         {696969, 6969696}};
    for (int i = 0; i < 3; ++i) {
        f[i]=HashFunctions(XOR_NUM[i][0]^rand_number,XOR_NUM[i][1]^rand_number);
    }
    return 0;
}