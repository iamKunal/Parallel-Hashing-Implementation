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

    HashFunctions() {
        this->c0 = this->c1 = 1;
    }

    HashFunctions(int c0, int c1) {
        this->c0 = c0;
        this->c1 = c1;
    }

    int g(int key) {
        return abs(((c0 + c1 * key) % 1900813) % TABLE_SIZE);
    }
};

int main() {
//    generateData();
    // Gets data from the input file.
    getData("/home/kunal/Documents/GITPRO/Parallel-Hashing-Implementation/test/input1");
////    Debugging purposes
//    for (auto x: keys) {
//        cout << x << ' ';
//    }
    int noOfBuckets = ceil(N / 409);

    vector<vector<vector<int> > > hashTable;
    hashTable.resize(noOfBuckets);
    // Sets up the Hash Tables
    for (int i = 0; i < noOfBuckets; ++i) {
        hashTable[i].resize(3);
        for (int j = 0; j < 3; ++j) {
            hashTable[i][j].resize(TABLE_SIZE, -1);
        }
    }

    // Check that bucket size is not exceeded while assigning
    vector<int> bucket_size(noOfBuckets, 0);
    for (int i = 0; i < N; ++i) {
        bucket_size[getBucketNumber(keys[i], noOfBuckets)]++;
    }
    if (*max_element(bucket_size.begin(), bucket_size.end()) > 512) {
        cout << "h() failed!" << endl;
        return 1;
    }
    HashFunctions f[3];
    // Numbers to be XOR'ed with random number to get corresponding functions.
    int XOR_NUM[3][2] = {{69,     696},
                         {6969,   69696},
                         {696969, 6969696}};

    omp_lock_t table_lock[noOfBuckets][3][TABLE_SIZE];
#pragma omp parallel for
    for (int i = 0; i < noOfBuckets; ++i) {
#pragma omp parallel for
        for (int j = 0; j < 3; ++j) {
#pragma omp parallel for
            for (int k = 0; k < TABLE_SIZE; ++k) {
                omp_init_lock(&table_lock[i][j][k]);
            }
        }
    }

    // Flag which tells if we need to change the hashing functions due to iterations>=25
    bool flag_change_g = false;
    srand(time(NULL));
    do {
        if (flag_change_g) {
            cout << "Last hashing failed, retrying with new random number." << endl;
        }

        // Set up hashing functions
        int rand_number = rand();   //Works well for 1804289383
        for (int i = 0; i < 3; ++i) {
            f[i] = HashFunctions(XOR_NUM[i][0] ^ rand_number, XOR_NUM[i][1] ^ rand_number);
        }

        cout << "Random number : " << rand_number << endl;
        flag_change_g = false;

#pragma omp parallel for
        for (int i = 0; i < N; ++i) {
            int bucketNumber = getBucketNumber(keys[i], noOfBuckets);
            int g[3];

            // Get the index in each Hash Table.
            for (int j = 0; j < 3; ++j) {
                g[j] = f[j].g(keys[i]);
            }

            int iterations = 0;

            // Cuckoo Hash in action.
            // Keep running until number is
            while (iterations < 25) {
                for (int j = 0; j < 3; ++j) {
                    if (hashTable[bucketNumber][0][g[0]] != keys[i] && hashTable[bucketNumber][1][g[1]] != keys[i] &&
                        hashTable[bucketNumber][2][g[2]] != keys[i]) {
                        omp_set_lock(&table_lock[bucketNumber][j][g[j]]);
                        hashTable[bucketNumber][j][g[j]] = keys[i];
                        omp_unset_lock(&table_lock[bucketNumber][j][g[j]]);
                    }
#pragma omp barrier
                    iterations++;
                }
            }
            if (hashTable[bucketNumber][0][g[0]] != keys[i] && hashTable[bucketNumber][1][g[1]] != keys[i] && hashTable[bucketNumber][2][g[2]] != keys[i]) {
#pragma omp critical
                flag_change_g = true;
            }
        }
    } while (flag_change_g);
    return 0;
}