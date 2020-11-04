#include "include/hashing.hpp"

Hashing::Hashing(){
    key = 0;
    hashValue = 0;
}
 
int Hashing::hashFunction(int key, vector<int> vector){
    
    int vectorSize = vector.size();

    hashValue = (key - 1) % vectorSize;

    cout<< "Hash value = " << hashValue << "\n";

    return (int)hashValue; 
}

LinkList::LinkList(){
    head = NULL;
    current = NULL;
    temp = NULL;
}

void LinkList::addNode(int addData){
    nodePointer n = new node;
    n->next = NULL;
    n->data = addData;
    if(head != NULL){
        current = head;
        while (current->next != NULL){
            current = current->next;
        }

        current->next = n; 

    }else{
        head = n;
    }
}

void LinkList::deleteNode(int deleteData){
    nodePointer delPointer = NULL;
    temp = head;
    current = head;
    while (current != NULL && current->data != deleteData){
        temp = current;
        current = current->next;
    }
    if(current == NULL){
        cout << deleteData <<"Is not in the list \n";
        delete delPointer; // to free the memory reserved for the pointer.

    }else{ //node found
        delPointer = current;
        current = current->next;
        temp->next = current;
        delete delPointer;
        cout << "The value: " <<deleteData << "was deleted. \n";
    }

}

void LinkList::printLinkList(){
    current = head;
    while (current != NULL){
        cout << current->data << "\n";
        current = current->next;
    }
    
}