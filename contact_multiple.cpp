#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <type_traits>
#include <vector>
using namespace std;
class Contact {
public:
    char fName[50], lName[50], address[50], email[50];
    long long phNo;

    void createContact() {
        cout << "Enter your first name: ";
        cin >> fName;
        cout << "Enter your last name: ";
        cin >> lName;
        cout << "Enter phone: ";
        cin >> phNo;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear buffer
        cout << "Enter address: ";
        cin.getline(address, 50);  // Allow multi-word addresses
        cout << "Enter email: ";
        cin >> email;
    }

    void modifyContact() {
        int choice;
        do {
            cout << "\nModify Contact Menu:" << endl;
            cout << "1. Modify Name" << endl;
            cout << "2. Modify Phone" << endl;
            cout << "3. Modify Address" << endl;
            cout << "4. Modify Email" << endl;
            cout << "5. Done" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch(choice) {
                case 1:
                    cout << "Enter new first name: ";
                    cin >> fName;
                    cout << "Enter new last name: ";
                    cin >> lName;
                    break;
                case 2:
                    cout << "Enter new phone number: ";
                    cin >> phNo;
                    break;
                case 3:
                    cout << "Enter new address: ";
                    cin.getline(address, 50);
                    break;
                case 4:
                    cout << "Enter new email: ";
                    cin >> email;
                    break;
                case 5:
                    cout << "Contact modified successfully!" << endl;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
            }
        } while (choice != 5);
    }

    void showContact() const {
        cout << "Name: " << fName << " " << lName << endl;
        cout << "Phone: " << phNo << endl;
        cout << "Address: " << address << endl;
        cout << "Email: " << email << endl;
    }
};

template <typename T>
class Node {
public:
    Contact data;
    Node* left;
    Node* right;
    vector<Node*> duplicates;  // Store nodes with same key
    T key;

    Node(Contact c, T k) : data(c), key(k), left(nullptr), right(nullptr) {}
};

template <typename T>
class ContactBST {
protected:
    Node<T>* root;

    void insert(Node<T>*& root, Contact c, T key) {
        if (root == nullptr) {
            root = new Node<T>(c, key);
        } else if (key == root->key) {
            // If key matches, add to duplicates vector
            root->duplicates.push_back(new Node<T>(c, key));
        } else if (key < root->key) {
            insert(root->left, c, key);
        } else {
            insert(root->right, c, key);
        }
    }

    void inorder(Node<T>* root) const {
        if (root != nullptr) {
            inorder(root->left);
            root->data.showContact();
            // Show all duplicates
            for (Node<T>* dup : root->duplicates) {
                cout << "\n--- Same " << getKeyType() << " Contact ---" << endl;
                dup->data.showContact();
            }
            cout << "===================" << endl;
            inorder(root->right);
        }
    }

    void searchAll(Node<T>* root, T key, bool& found) const {
        if (root != nullptr) {
            if (root->key == key) {
                found = true;
                root->data.showContact();
                // Show all duplicates when key matches
                for (Node<T>* dup : root->duplicates) {
                    cout << "\n--- Same " << getKeyType() << " Contact ---" << endl;
                    dup->data.showContact();
                }
                cout << "===================" << endl;
                searchAll(root->left, key, found);
                searchAll(root->right, key, found);
            } else {
                if (key < root->key) {
                    searchAll(root->left, key, found);
                } else {
                    searchAll(root->right, key, found);
                }
            }
        }
    }

    Node<T>* search(Node<T>* root, T key) const {
        if (root == nullptr || root->key == key)
            return root;

        if (key < root->key)
            return search(root->left, key);

        return search(root->right, key);
    }

    void writeToFile(Node<T>* root, ofstream& file) const {
        if (root != nullptr) {
            file.write(reinterpret_cast<const char*>(&root->data), sizeof(Contact));
            // Write duplicates
            for (Node<T>* dup : root->duplicates) {
                file.write(reinterpret_cast<const char*>(&dup->data), sizeof(Contact));
            }
            writeToFile(root->left, file);
            writeToFile(root->right, file);
        }
    }

    void readFromFile(ifstream& file) {
        Contact c;
        while (file.read(reinterpret_cast<char*>(&c), sizeof(Contact))) {
            T key = extractKey(c);
            insert(root, c, key);
        }
    }

    Node<T>* findMin(Node<T>* node) const {
        Node<T>* current = node;
        while (current && current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    Node<T>* deleteNode(Node<T>* root, T key) {
        if (root == nullptr) {
            return root;
        }

        if (key < root->key) {
            root->left = deleteNode(root->left, key);
        }
        else if (key > root->key) {
            root->right = deleteNode(root->right, key);
        }
        else {
            // If there are duplicates, remove the first one
            if (!root->duplicates.empty()) {
                Node<T>* dupNode = root->duplicates.back();
                root->duplicates.pop_back();
                delete dupNode;
                return root;
            }

            if (root->left == nullptr) {
                Node<T>* temp = root->right;
                delete root;
                return temp;
            }
            else if (root->right == nullptr) {
                Node<T>* temp = root->left;
                delete root;
                return temp;
            }

            Node<T>* temp = findMin(root->right);
            root->data = temp->data;
            root->key = temp->key;
            root->duplicates = temp->duplicates;
            root->right = deleteNode(root->right, temp->key);
        }
        return root;
    }

    void deleteTree(Node<T>* node) {
        if (node != nullptr) {
            deleteTree(node->left);
            deleteTree(node->right);
            // Delete all duplicates
            for (Node<T>* dup : node->duplicates) {
                delete dup;
            }
            delete node;
        }
    }

    virtual string getKeyType() const {
        return "Key";
    }

public:
    ContactBST() : root(nullptr) {}
    
    ~ContactBST() {
        deleteTree(root);
    }

    void insertContact(Contact c, T key) {
        insert(root, c, key);
    }

    void showAllContacts() const {
        if (root == nullptr) {
            cout << "No contacts found!" << endl;
            return;
        }
        inorder(root);
    }

    Node<T>* searchContact(T key) const {
        return search(root, key);
    }

    void displayContact(T key) const {
        bool found = false;
        searchAll(root, key, found);
        if (!found) {
            cout << "No contacts found!" << endl;
        }
    }

    void writeContactsToFile(const string& filename) const {
        ofstream file(filename, ios::binary);
        if (!file) {
            cout << "Error opening file for writing!" << endl;
            return;
        }
        writeToFile(root, file);
        file.close();
    }

    void readContactsFromFile(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file) {
            cout << "Error opening file for reading!" << endl;
            return;
        }
        readFromFile(file);
        file.close();
    }

    void deleteContact(T key) {
        root = deleteNode(root, key);
    }

    virtual T extractKey(Contact c) const {
        if constexpr (is_same<T, long long>::value) {
            return c.phNo;
        } else {
            return T();
        }
    }
};

class ContactBSTByName : public ContactBST<string> {
public:
    string extractKey(Contact c) const override {
        return string(c.fName) + " " + string(c.lName);
    }
    string getKeyType() const override {
        return "Name";
    }
};

class ContactBSTByEmail : public ContactBST<string> {
public:
    string extractKey(Contact c) const override {
        return string(c.email);
    }
    string getKeyType() const override {
        return "Email";
    }
};

class ContactBSTByAddress : public ContactBST<string> {
public:
    string extractKey(Contact c) const override {
        return string(c.address);
    }
    string getKeyType() const override {
        return "Address";
    }
};

int main() {
    ContactBST<long long> bstByPhone;
    ContactBSTByName bstByName;
    ContactBSTByEmail bstByEmail;
    ContactBSTByAddress bstByAddress;

    // Load existing contacts if any
    bstByPhone.readContactsFromFile("contacts_phone.dat");
    bstByName.readContactsFromFile("contacts_name.dat");
    bstByEmail.readContactsFromFile("contacts_email.dat");
    bstByAddress.readContactsFromFile("contacts_address.dat");

    int choice;
    do {
        cout << "\n=== Contact Management System ===" << endl;
        cout << "1. Add Single Contact" << endl;
        cout << "2. Add Multiple Contacts" << endl;
        cout << "3. Display All Contacts" << endl;
        cout << "4. Search by Phone" << endl;
        cout << "5. Search by Name" << endl;
        cout << "6. Search by Email" << endl;
        cout << "7. Search by Address" << endl;
        cout << "8. Modify Contact" << endl;
        cout << "9. Delete Contact by Phone" << endl;
        cout << "10. Save and Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                Contact c;
                c.createContact();
                bstByPhone.insertContact(c, c.phNo);
                bstByName.insertContact(c, bstByName.extractKey(c));
                bstByEmail.insertContact(c, bstByEmail.extractKey(c));
                bstByAddress.insertContact(c, bstByAddress.extractKey(c));
                cout << "Contact added successfully!" << endl;
                break;
            }
            case 2: {
                int numContacts;
                cout << "How many contacts do you want to add? ";
                cin >> numContacts;
                for(int i = 1; i <= numContacts; i++) {
                    cout << "\nEntering details for contact " << i << ":" << endl;
                    Contact c;
                    c.createContact();
                    bstByPhone.insertContact(c, c.phNo);
                    bstByName.insertContact(c, bstByName.extractKey(c));
                    bstByEmail.insertContact(c, bstByEmail.extractKey(c));
                    bstByAddress.insertContact(c, bstByAddress.extractKey(c));
                    cout << "Contact " << i << " added successfully!" << endl;
                }
                break;
            }
            case 3: {
                cout << "\nContacts sorted by phone number:" << endl;
                bstByPhone.showAllContacts();
                break;
            }
            case 4: {
                long long phoneToSearch;
                cout << "Enter phone number to search: ";
                cin >> phoneToSearch;
                bstByPhone.displayContact(phoneToSearch);
                break;
            }
            case 5: {
                string nameToSearch;
                cout << "Enter name to search (First Last): ";
                cin.ignore();
                getline(cin, nameToSearch);
                bstByName.displayContact(nameToSearch);
                break;
            }
            case 6: {
                string emailToSearch;
                cout << "Enter email to search: ";
                cin >> emailToSearch;
                bstByEmail.displayContact(emailToSearch);
                break;
            }
            case 7: {
                string addressToSearch;
                cout << "Enter address to search: ";
                cin.ignore();
                getline(cin, addressToSearch);
                cout << "\nContacts at address: " << addressToSearch << endl;
                cout << "===================" << endl;
                bstByAddress.displayContact(addressToSearch);
                break;
            }
            case 8: {
                long long phoneToModify;
                cout << "Enter phone number of contact to modify: ";
                cin >> phoneToModify;
                Node<long long>* nodeToModify = bstByPhone.searchContact(phoneToModify);
                if (nodeToModify != nullptr) {
                    Contact oldContact = nodeToModify->data;
                    Contact newContact = oldContact;
                    
                    newContact.modifyContact();
                    
                    bstByPhone.deleteContact(phoneToModify);
                    bstByName.deleteContact(bstByName.extractKey(oldContact));
                    bstByEmail.deleteContact(bstByEmail.extractKey(oldContact));
                    bstByAddress.deleteContact(bstByAddress.extractKey(oldContact));
                    
                    bstByPhone.insertContact(newContact, newContact.phNo);
                    bstByName.insertContact(newContact, bstByName.extractKey(newContact));
                    bstByEmail.insertContact(newContact, bstByEmail.extractKey(newContact));
                    bstByAddress.insertContact(newContact, bstByAddress.extractKey(newContact));
                    
                    cout << "Contact modified successfully!" << endl;
                } else {
                    cout << "Contact not found!" << endl;
                }
                break;
            }
            case 9: {
                long long phoneToDelete;
                cout << "Enter phone number to delete: ";
                cin >> phoneToDelete;
                Node<long long>* nodeToDelete = bstByPhone.searchContact(phoneToDelete);
                if (nodeToDelete != nullptr) {
                    Contact tempContact = nodeToDelete->data;
                    bstByPhone.deleteContact(phoneToDelete);
                    bstByName.deleteContact(bstByName.extractKey(tempContact));
                    bstByEmail.deleteContact(bstByEmail.extractKey(tempContact));
                    bstByAddress.deleteContact(bstByAddress.extractKey(tempContact));
                    cout << "Contact deleted successfully!" << endl;
                } else {
                    cout << "Contact not found!" << endl;
                }
                break;
            }
            case 10:
                bstByPhone.writeContactsToFile("contacts_phone.dat");
                bstByName.writeContactsToFile("contacts_name.dat");
                bstByEmail.writeContactsToFile("contacts_email.dat");
                bstByAddress.writeContactsToFile("contacts_address.dat");
                cout << "Contacts saved. Thank you for using Contact Management System!" << endl;
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 10);

    return 0;
}
