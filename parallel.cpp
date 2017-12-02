//
// Created by kunal on 19/11/17.
//

#include <bits/stdc++.h>
#include <omp.h>

using namespace std;


int N = 1'00'000;
//#define TABLE_SIZE 192
int TABLE_SIZE = 192;

template<class T>
bool contains(const std::vector<T> &vec, const T &value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

vector<unsigned int> keys;
int h_number = 69;

void generateData() {
    vector<unsigned int> keys;
    unsigned int num;
    unsigned int maxValue = INT_MAX;
    fstream input_data("/home/kunal/Documents/GITPRO/Parallel-Hashing-Implementation/test/input2", ios::out);
    if (!input_data) {
        cout << "IO Error";
        return;
    }
    for (unsigned int i = 0; i < N; ++i) {
        if (i % 10'000 == 0)
            cerr << i << endl;
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
    unsigned int num;
    for (unsigned int i = 0; i < N; ++i) {
        input_data >> num;
        keys.push_back(num);
    }
    input_data.close();
}

unsigned int getBucketNumber(unsigned int key, unsigned int noOfBuckets) {
    unsigned int c1 = 69696969 ^::h_number;
    unsigned int c0 = 696969 ^::h_number;
    unsigned int prime = 1900813;

    c1 = 1;
    c0 = 0;
    prime = key + 1;
    return (((c0 + c1 * key) % prime) % noOfBuckets);       // Else returning negative values.
}

class HashFunctions {
public:

    unsigned int c0, c1;

    HashFunctions() {
        this->c0 = this->c1 = 1;
    }

    HashFunctions(unsigned int c0, unsigned int c1) {
        this->c0 = c0;
        this->c1 = c1;
    }

    unsigned int g(unsigned int key) {
        return (((c0 + c1 * key) % 1900813) % TABLE_SIZE);       // Else returning negative values.
    }
};

int main(int argc, char **argv) {
    if (argc == 3) {
        N = atoi(argv[1]);
        TABLE_SIZE = atoi(argv[2]);
    }
    // Gets data from the input file.
//    getData("/home/kunal/Documents/GITPRO/Parallel-Hashing-Implementation/test/input2"); // Contains 1'00'00'000 unique integers
    for (int i = 0; i < N; ++i) {
        keys.push_back(i);
    }
    unsigned int noOfBuckets = N / 409 + 1;

    vector<vector<vector<unsigned int> > > hashTable;
    hashTable.resize(noOfBuckets);
    // Sets up the Hash Tables
#pragma omp parallel for
    for (unsigned int i = 0; i < noOfBuckets; ++i) {
        hashTable[i].resize(3);
#pragma omp parallel for
        for (unsigned int j = 0; j < 3; ++j) {
            hashTable[i][j].resize(TABLE_SIZE, -1);
            fill(hashTable[i][j].begin(), hashTable[i][j].end(), -1);
        }
    }
    vector<unsigned int> bucket_size(noOfBuckets, 0);
    srand(time(NULL));
    h_number = rand();
    // Check that bucket size is not exceeded while assigning
    fill(bucket_size.begin(), bucket_size.end(), 0);
    for (unsigned int i = 0; i < N; ++i) {
        bucket_size[getBucketNumber(keys[i], noOfBuckets)]++;
    }

    if (*max_element(bucket_size.begin(), bucket_size.end()) > 512) {
        cout << "h() failed!" << endl;
        return 1;
    }
    cout << "Max bucket size : " << *max_element(bucket_size.begin(), bucket_size.end()) << endl;
    cout << "Min bucket size : " << *min_element(bucket_size.begin(), bucket_size.end()) << endl;
    HashFunctions f[3];
    // Numbers to be XOR'ed with random number to get corresponding functions.
    unsigned int XOR_NUM[3][2] = {{69,     696},
                                  {6969,   69696},
                                  {696969, 6969696}};

    omp_lock_t table_lock[noOfBuckets][3][TABLE_SIZE];
#pragma omp parallel for
    for (unsigned int i = 0; i < noOfBuckets; ++i) {
#pragma omp parallel for
        for (unsigned int j = 0; j < 3; ++j) {
#pragma omp parallel for
            for (unsigned int k = 0; k < TABLE_SIZE; ++k) {
                omp_init_lock(&table_lock[i][j][k]);
            }
        }
    }

    // Flag which tells if we need to change the hashing functions due to iterations>=25
    bool flag_change_g = false;
    srand(time(NULL));
    unsigned int iterations = 0;
    unsigned int left_out = 0;
    int number = 0;
    do {
        if (flag_change_g) {
            cout << "Number of keys left out : " << left_out << endl;
            cout << "Last hashing failed, retrying with new random number." << endl;
#pragma omp parallel for
            for (unsigned int i = 0; i < noOfBuckets; ++i) {
#pragma omp parallel for
                for (unsigned int j = 0; j < 3; ++j) {
                    fill(hashTable[i][j].begin(), hashTable[i][j].end(), -1);
                }
            }
        }
        // Set up hashing functions
        unsigned int rand_number = rand();
        for (unsigned int i = 0; i < 3; ++i) {
            f[i] = HashFunctions(XOR_NUM[i][0] ^ rand_number, XOR_NUM[i][1] ^ rand_number);
        }

        cout << "Random number : " << rand_number << endl;

        // Set up flags.
        flag_change_g = false;
        left_out = 0;
        iterations = 0;
        double start = omp_get_wtime();
        // Cuckoo Hash in action.
        // Keep running until number is saved in the table or infinite loop (iterations>25) is reached
        while (iterations < 25) {
#pragma omp parallel for
            for (unsigned int i = 0; i < N; ++i) {
                unsigned int bucketNumber = getBucketNumber(keys[i], noOfBuckets);
                unsigned int tableNumber = iterations % 3;
                unsigned int g[3];

                // Get the index in each Hash Table.
                for (unsigned int j = 0; j < 3; ++j) {
                    g[j] = f[j].g(keys[i]);
                }
                if (hashTable[bucketNumber][0][g[0]] != keys[i] && hashTable[bucketNumber][1][g[1]] != keys[i] &&
                    hashTable[bucketNumber][2][g[2]] != keys[i]) {
                    omp_set_lock(&table_lock[bucketNumber][tableNumber][g[tableNumber]]);
                    hashTable[bucketNumber][tableNumber][g[tableNumber]] = keys[i];
                    omp_unset_lock(&table_lock[bucketNumber][tableNumber][g[tableNumber]]);
                }
            }
            iterations++;
        }
        cout << "Current Time : " << omp_get_wtime() - start << endl;
        // This part checks whether any of the keys are left out, if yes then regenerate the hashing functions.
#pragma omp parallel for
        for (unsigned int i = 0; i < N; ++i) {
            unsigned int bucketNumber = getBucketNumber(keys[i], noOfBuckets);
            unsigned int g[3];

            // Get the index in each Hash Table.
            for (unsigned int j = 0; j < 3; ++j) {
                g[j] = f[j].g(keys[i]);
            }
            if (hashTable[bucketNumber][0][g[0]] != keys[i] && hashTable[bucketNumber][1][g[1]] != keys[i] &&
                hashTable[bucketNumber][2][g[2]] != keys[i]) {
#pragma omp critical
                {
                    // Count of left out keys
                    left_out++;
                    flag_change_g = true;
                }
            }
        }
        number++;
        if(!flag_change_g)
            cout<<"Hashed Successfully!"<<endl;
        if (number >= 100){
            break;
        }
    } while (1);
    return 0;
}
