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

vector<unsigned int> keys;

/*void generateData() {
    vector<unsigned int> keys;
    unsigned int num;
    unsigned int maxValue = INT_MAX;
    fstream input_data("/Desktop/test.txt", ios::out);
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
}*/

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
    unsigned int c1 ;
    unsigned int c0 ;
    unsigned int prime = 1900813;
    c1=324;
    c0=2133;
    /*c1 = 1;
    c0 = 0;
    prime = key + 1;*/
    //changed
    return (((c0 + c1 * key) % prime) % noOfBuckets);       // Else returning negative values.
}

class HashFunctions {
public:

    unsigned int c0, c1;

    HashFunctions() {
        /*this->c0 = this->c1 = 1;*/  //changed
        this->c0=rand()%1000;
        this->c1=rand()%1000;
    }

    HashFunctions(unsigned int c0, unsigned int c1) {
        this->c0 = c0;
        this->c1 = c1;
    }

    unsigned int g(unsigned int key) {
        return (((c0 + c1 * key) % 1900813) % TABLE_SIZE);       // Else returning negative values.
    }
};

int main() {
    // Gets data from the input file.
    getData("test.txt"); // Contains 1'00'00'000 unique integers
    int j,k,i;
    unsigned int noOfBuckets =  N/409;
    noOfBuckets=noOfBuckets+1;
    cout<<noOfBuckets<<endl;
    vector<int> vec1;
    vector<vector<int> > vec2;
    vector<vector<vector<int> > > hashTable;
    //hashTable.resize(noOfBuckets);
    cout<<"a";
    for(i=0;i<191;i++)
        vec1.push_back(0);
    for(i=0;i<3;i++)
        vec2.push_back(vec1);
    cout<<noOfBuckets<<endl;
    for(i=0;i<noOfBuckets;i++)
        hashTable.push_back(vec2);
    // Sets up the Hash Tables
/*#pragma omp parallel for
    for (unsigned int i = 0; i < noOfBuckets; ++i) {
        hashTable[i].resize(3);
#pragma omp parallel for
        for (unsigned int j = 0; j < 3; ++j) {
            hashTable[i][j].resize(TABLE_SIZE,0);
            fill(hashTable[i][j].begin(), hashTable[i][j].end(), 0);
        }
    }*/
    //cout<<"a";
    //cout<<"a";
    // Check that bucket size is not exceeded while assigning
    vector<unsigned int> bucket_size;
    for(i=0;i<noOfBuckets;i++)
        bucket_size.push_back(0);
    for (unsigned int i = 0; i < N; ++i) {
        bucket_size[getBucketNumber(keys[i], noOfBuckets)]++;
    }
    if (*max_element(bucket_size.begin(), bucket_size.end()) > 512) {
        cout << "h() failed!" << endl;
        return 1;
    }
    HashFunctions f[3];
    // Numbers to be XOR'ed with random number to get corresponding functions.
    unsigned int XOR_NUM[3][2] = {{69,     696},
                         {6969,   69696},
                         {696969, 6969696}};

    /*omp_lock_t table_lock[noOfBuckets][3][TABLE_SIZE];
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
*/
    // Flag which tells if we need to change the hashing functions due to iterations>=25
   // bool flag_change_g = false;
   // srand(time(NULL));
    unsigned int iterations = 0;
    //unsigned int left_out = 0;
    unsigned int g[3];
    
    /*
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
        unsigned int rand_number = rand()%10000;
        for (unsigned int i = 0; i < 3; ++i) {
            f[i] = HashFunctions(XOR_NUM[i][0] ^ rand_number, XOR_NUM[i][1] ^ rand_number);
        }

        cout << "Random number : " << rand_number << endl;

        // Set up flags.
        flag_change_g = false;
        left_out = 0;
        iterations = 0;

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
    } while (flag_change_g);*/
    unsigned int sub_buck;
 while(1)
 {
     vector<int> remaining_keys;
     int n1,n2,iterations;
    unsigned int rand_number = rand()%10000;
    for (unsigned int i = 0; i < 3; ++i) {
        f[i] = HashFunctions(XOR_NUM[i][0] ^ rand_number, XOR_NUM[i][1] ^ rand_number);
    }
    for(i=0;i<N;i++)
    {
        unsigned int bucketNumber = getBucketNumber(keys[i], noOfBuckets);
        for(j=0;j<3;j++)
        g[j] = f[j].g(keys[i]);
         if(hashTable[bucketNumber][0][g[0]]==0)
             hashTable[bucketNumber][0][g[0]]=keys[i];
        else if(hashTable[bucketNumber][1][g[1]]==0)
            hashTable[bucketNumber][1][g[1]]=keys[i];
        else if(hashTable[bucketNumber][2][g[2]]==0)
            hashTable[bucketNumber][2][g[2]]=keys[i];
        else
            remaining_keys.push_back(keys[i]);
    }
    for(i=0;i<remaining_keys.size();i++)
    {
        unsigned int bucketNumber = getBucketNumber(remaining_keys[i], noOfBuckets);
        sub_buck=rand()%3;
        g[sub_buck] = f[sub_buck].g(remaining_keys[i]);
        n1=hashTable[bucketNumber][sub_buck][g[sub_buck]];
        hashTable[bucketNumber][sub_buck][g[sub_buck]]=remaining_keys[i];
        iterations = 0;
        while(iterations<=25)
        {
            for(j=0;j<3;j++)
            {
                if(j!=sub_buck)
                {
                    bucketNumber= getBucketNumber(n1, noOfBuckets);
                    g[j]=f[j].g(n1);
                    if(hashTable[bucketNumber][j][g[j]]==0)
                    {
                        hashTable[bucketNumber][j][g[j]]=n1;
                        break;
                    }

                }
           }
            if(j==3)
            {
                for(j=0;j<3;j++)
                {
                    if(j!=sub_buck)
                    {
                        n2=hashTable[bucketNumber][j][g[j]];
                        hashTable[bucketNumber][j][g[j]]=n1;
                        n1=n2;
                        sub_buck=j;
                        iterations=iterations+1;
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }
          if(iterations>25)
          {
              break;
          }
    }
         if(i==remaining_keys.size())
         {
              #pragma omp parallel for
             for (unsigned int i = 0; i < noOfBuckets; ++i) {
                #pragma omp parallel for
                 for (unsigned int j = 0; j < 3; ++j) {
                     fill(hashTable[i][j].begin(), hashTable[i][j].end(), 0);
                 }
             }
             cout << "Number of keys left out : " <<remaining_keys.size()-i  << endl;
             cout << "Last hashing failed, retrying with new random number." << endl;
             continue;
         }
          else
          {
              cout<<"Successfull"<<endl;
              break;
          }
}
    return 0;
}
