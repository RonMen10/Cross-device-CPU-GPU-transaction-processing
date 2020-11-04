#include <iostream>
#include <vector>
#include <list>
#include <cstdlib>
#include <array>
using namespace std;

class Hashing{
    public:
    Hashing(); //constructor
    
    
    int key;
    int hashValue;
    int hashFunction(int key, vector<int> vector);
};

class LinkList{
    public:
        typedef struct node{
            int data;
            int hashValue;
            node* left;
            node* next;
        }*nodePointer; // helps to use "nodePointer" instead of "node*" every time we want to use the pointer
        
        

        nodePointer head;
        nodePointer current;
        nodePointer temp;

    public:

        LinkList();//Constructor
        void addNode(int addData);
        void deleteNode(int deleteData); 
        void printLinkList();
        void addList();
        
};


class MainArray{
    public:
    int arraySize;
    MainArray();//Constructor
    LinkList arrayNode;
    LinkList createArray(vector<int> data);//Hash Build
    LinkList createTableJoin(vector<int> data);//Hash Join
};