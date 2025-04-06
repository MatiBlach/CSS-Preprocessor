#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
using namespace std;

const int T = 29;
const int MAX_STR = 20;

enum Mode {
    Command,
    ParseSelector,
    ParseProperty,
    ParseValue,
};

struct Selector {
    char* name;
    struct Selector* next;
};

struct Attribute {
    char* property;
    char* value;
    struct Attribute* next;
};

struct Section {
    Selector* SelectorList = nullptr;
    Attribute* AttributeList = nullptr;

    void addSelector(const char* name) {
        Selector* newSelector = new Selector;

        newSelector->name = new char[strlen(name) + 1];
        strcpy(newSelector->name, name);

        newSelector->next = nullptr;

        if (SelectorList == nullptr) {
            SelectorList = newSelector;
        }
        else {
            Selector* current = SelectorList;
            while (current->next != nullptr) {
                if (strcmp(current->name, name) == 0) {
                    delete newSelector;
                    return;
                }
                current = current->next;
            }
            if (strcmp(current->name, name) == 0) {
                delete newSelector;
                return;
            }
            current->next = newSelector;
        }
    }

    void addAttribute(const char* property, const char* value) {

        Attribute* newAttribute = new Attribute;

        newAttribute->property = new char[strlen(property) + 1];
        newAttribute->value = new char[strlen(value) + 1];

        strcpy(newAttribute->property, property);
        strcpy(newAttribute->value, value);

        newAttribute->next = nullptr;

        if (AttributeList == nullptr) {
            AttributeList = newAttribute;
            return;
        }


        Attribute* current = AttributeList;
        Attribute* previous = nullptr;
        while (current != nullptr && strcmp(current->property, property) != 0) {
            previous = current;
            current = current->next;
        }


        if (current != nullptr) {

            if (current->next == nullptr) {
                delete[] current->value;
                current->value = new char[strlen(value) + 1];
                strcpy(current->value, value);
            }

            else if (previous == nullptr) {
                AttributeList = current->next;
                delete[] current->property;
                delete[] current->value;
                delete current;
                addAttribute(property, value);
            }

            else {
                previous->next = current->next;
                delete[] current->property;
                delete[] current->value;
                delete current;
                addAttribute(property, value);
            }
        }

        else {
            current = AttributeList;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newAttribute;
        }
    }

    Selector* findSelector(int i) {
        int j = 1;
        Selector* selector = SelectorList;
        while (selector != nullptr) {
            if (j == i) {
                return selector;
            }
            j++;
            selector = selector->next;
        }
        return nullptr;
    }


    Attribute* findAttributeByProperty(const char* property) {
        Attribute* current = nullptr;
        if (AttributeList != nullptr)
            current = AttributeList;
        while (current != nullptr) {
            if (strcmp(current->property, property) == 0) //equal
                return current;
            current = current->next;
        }
        return nullptr;
    }


};


struct Block {
    Section sections[T];
    bool sectionV[T] = {};
    struct Block* prev = nullptr;
    struct Block* next = nullptr;

    Block* getTail(Block*& blockListHead) {
        if (blockListHead == nullptr)
            return nullptr;
        Block* current = blockListHead;
        while (current->next != nullptr)
            current = current->next;
        return current;
    }

    bool isSectionUsed(int pos) {
        return sectionV[pos];
    }

};


struct String {
    int size = MAX_STR;
    int length = 0;
    char* string;

    String() {
        size = MAX_STR;
        length = 0;
        string = new char[MAX_STR];
    }

    void expandString() {
        size *= 2;
        char* expandedString = new char[size];
        memcpy(expandedString, string, length + 1);
        delete[] string;
        string = expandedString;
    }

    void addChar(int i, char c) {
        string[i] = c;
        length++;
    }

    ~String() {
        if (string != nullptr) {
            delete[] string;
            string = nullptr;
        }
    }
};

void addNewBlock(Block*& blockListHead) {
    Block* current = blockListHead;
    while (current->next != nullptr) {
        current = current->next;
    }
    Block* newBlock = new Block;
    current->next = newBlock;
    newBlock->prev = current;
}

Block* removeBlock(Block*& blockListHead, Block*& toRemove) {
    if (blockListHead == nullptr || toRemove == nullptr)
        return blockListHead;
    else if (blockListHead->next == nullptr)
    {
        return blockListHead;
    }
    else if (toRemove == blockListHead) {
        (blockListHead->next)->prev = nullptr;
        return blockListHead->next;
    }
    else if (toRemove->next == nullptr)
    {
        (toRemove->prev)->next = nullptr;
        delete toRemove;
        return blockListHead->next;
    }
    else {
        (toRemove->prev)->next = toRemove->next;
        (toRemove->next)->prev = toRemove->prev;
        delete toRemove;
        return blockListHead;
    }

}

Section* findSection(Block* blockListHead, int i) {
    int counter = 1;
    Block* current = blockListHead;
    while (current != nullptr) {
        for (int j = 0; j < T; j++) {
            if (current->isSectionUsed(j)) {
                if (counter == i) {
                    return &(current->sections[j]);
                }
                counter++;
            }
            if (counter > i) {
                return nullptr;
            }
        }
        current = current->next;
    }
    return nullptr;
}

Section* findNewestSelector(Block* blockListHead, const char* name) {
    if (blockListHead == nullptr) {
        return nullptr;
    }

    Block* current = blockListHead->getTail(blockListHead);
    while (current != nullptr) {
        for (int i = T - 1; i >= 0; i--) {
            if (current->isSectionUsed(i)) {
                Selector* selector = current->sections[i].SelectorList;
                while (selector != nullptr) {
                    if (strcmp(selector->name, name) == 0) {
                        return &(current->sections[i]);
                    }
                    selector = selector->next;
                }
            }
        }
        current = current->prev;
    }
    return nullptr;
}

int toInt(const char* str, int pos, int end) {
    int r = 0;
    int m = 1;
    for (int i = end; i >= pos; i--) {
        r = r + ((int)str[i] - 48) * m;
        m *= 10;
    }

    return r;
}

int countSelectors(const Section* section) {
    int count = 0;
    Selector* current = section->SelectorList;
    while (current != nullptr) {
        count++;
        current = current->next;
    }
    return count;
}

int countAttributes(const Section* section) {
    int count = 0;
    Attribute* current = section->AttributeList;
    while (current != nullptr) {
        count++;
        current = current->next;
    }
    return count;
}

int countSections(Block* blockListHead) {
    int count = 0;
    Block* current = blockListHead;
    while (current != nullptr) {
        for (int i = 0; i < T; i++) {
            if (current->isSectionUsed(i)) {
                count++;
            }
        }
        current = current->next;
    }
    delete current;
    return count;
}

int countAttribute(Block* blockListHead, const char* attr) {
    int count = 0;
    Block* current = blockListHead;
    while (current != nullptr) {
        for (int i = 0; i < T; i++) {
            if (current->isSectionUsed(i)) {
                Section* section = &(current->sections[i]);
                Attribute* attribute = section->AttributeList;
                while (attribute != nullptr) {
                    if (strcmp(attribute->property, attr) == 0) {
                        count++;
                    }
                    attribute = attribute->next;
                }
            }
        }
        current = current->next;
    }
    return count;
}

int countSelector(Block* blockListHead, const char* sel) {
    int count = 0;
    Block* current = blockListHead;
    while (current != nullptr) {
        for (int i = 0; i < T; i++) {
            if (current->isSectionUsed(i)) {
                Section* section = &(current->sections[i]);
                Selector* selector = section->SelectorList;
                while (selector != nullptr) {
                    if (strcmp(selector->name, sel) == 0) {
                        count++;
                    }
                    selector = selector->next;
                }
            }
        }
        current = current->next;
    }
    return count;
}
bool removeSection(Block*& blockListHead, Section* section) {


    Block* current = blockListHead;

    while (current != nullptr) {

        if (section == nullptr) {
            return false;
        }

        for (int i = 0; i < T; i++) {
            if (&current->sections[i] == section) {
                current->sectionV[i] = false;
                if (section->SelectorList != nullptr && section->AttributeList != nullptr) {
                    section->SelectorList = nullptr;
                    section->AttributeList = nullptr;
                }
                bool rmBlock = true;
                for (int j = 0; j < T; j++) {
                    if (current->sectionV[j]) //{}
                        rmBlock = false;
                }
                if (rmBlock)
                    blockListHead = removeBlock(blockListHead, current);
                return true;
            }
        }
        current = current->next;
    }

    return false;
}

bool removeAttribute(Block* blockListHead, Section* section, const char* prop) {
    Attribute* attribute = section->AttributeList;
    Attribute* prev = nullptr;

    while (attribute != nullptr) {
        if (strcmp(attribute->property, prop) == 0) {
            if (prev == nullptr) {
                section->AttributeList = attribute->next;
            }
            else {
                prev->next = attribute->next;
            }
            delete attribute;

            if (section->AttributeList == nullptr) {
                removeSection(blockListHead, section);
            }

            return true;
        }

        prev = attribute;
        attribute = attribute->next;
    }
    return false;
}

void freeBlockList(Block*& blockListHead) {
    Block* current = blockListHead;
    while (current != nullptr) {
        Block* temp = current;

        for (int i = 0; i < T; i++) {
            Selector* currentSelector = temp->sections[i].SelectorList;
            while (currentSelector != nullptr) {
                Selector* tempSelector = currentSelector;
                currentSelector = currentSelector->next;
                delete[] tempSelector->name;
                delete tempSelector;
            }
        }

        for (int i = 0; i < T; i++) {
            Attribute* currentAttribute = temp->sections[i].AttributeList;
            while (currentAttribute != nullptr) {
                Attribute* tempAttribute = currentAttribute;
                currentAttribute = currentAttribute->next;
                delete[] tempAttribute->property;
                delete[] tempAttribute->value;
                delete tempAttribute;
            }
        }
        current = current->next;

        delete temp;
       
    }
    blockListHead = nullptr;
}

int main()
{
    Block* BlockList = new Block; // Head
    Block* CurrentBlock = BlockList;

    Mode mode = ParseSelector;
    String* str = new String();
    String* str2 = new String();
    int str_i = 0;
    int str2_i = 0;
    int sec_i = 0;
    int com1_i = -1;
    int com2_i = -1;
    int j = 0;
    int i = 0;
    char c = ' ';
    char* z;
    char* n;

    while (c != EOF) {
        if (str_i > 3 && str->string[0] == '?' && str->string[1] == '?' && str->string[2] == '?' && str->string[3] == '?') {
            mode = Command;
            str_i = 0;
            delete str;
            str = new String();
        }

        c = getchar();
        if (sec_i == 8) {
            addNewBlock(BlockList);
            CurrentBlock = CurrentBlock->next;
            sec_i = 0;
        }
        if (mode == ParseSelector) {
            if (c != '{') {
                if (c != ',') {
                    if (isspace(c))
                        c = ' ';
                    if (str_i == 0 && isspace(c) || str_i != 0 && isspace(str->string[str_i - 1]) && isspace(c))
                    {
                    }
                    else {

                        if (str->length + 1 < str->size) {
                            str->addChar(str_i, c);
                            str_i++;
                        }
                        else {
                            str->expandString();
                            str->addChar(str_i, c);
                            str_i++;
                        }
                    }
                }
                else {
                    if (str->string[str_i - 1] == ' ') {
                        str->string[str_i - 1] = '\0';
                        str->length--;
                    }
                    else {
                        str->string[str_i] = '\0';
                    }
                    CurrentBlock->sections[sec_i].addSelector(str->string);
                    CurrentBlock->sectionV[sec_i] = true;
                    delete str;
                    str = new String();
                    str_i = 0;
                }
            }
            else {
                mode = ParseProperty;
                if (str->string[str_i - 1] == ' ') {
                    str->string[str_i - 1] = '\0';
                    str->length--;
                }
                else {
                    str->string[str_i] = '\0';
                }
                CurrentBlock->sections[sec_i].addSelector(str->string);
                CurrentBlock->sectionV[sec_i] = true;
                delete str;
                str = new String();
                str_i = 0;
            }
        }
        else if (mode == ParseProperty) {
            if (c != ':' && c != '}') {
                if (isspace(c))
                    c = ' ';
                if (str2_i == 0 && isspace(c) || str2_i != 0 && isspace(str2->string[str2_i - 1]) && isspace(c))
                {
                }
                else {

                    if (str2->length + 1 < str2->size) {
                        str2->addChar(str2_i, c);
                        str2_i++;
                    }
                    else {
                        str2->expandString();
                        str2->addChar(str2_i, c);
                        str2_i++;
                    }
                }
            }
            else {
                if (c == ':') {
                    mode = ParseValue;
                    str2->string[str2_i] = '\0';
                }
                else if (c == '}') {
                    mode = ParseSelector;
                    sec_i++;

                }


            }
        }
        else if (mode == ParseValue) {
            if (c != ';' && c != '}') {
                if (isspace(c))
                    c = ' ';
                if (str_i == 0 && isspace(c) || str_i != 0 && isspace(str->string[str_i - 1]) && isspace(c))
                {
                }
                else {

                    if (str->length + 1 < str->size) {
                        str->addChar(str_i, c);
                        str_i++;
                    }
                    else {
                        str->expandString();
                        str->addChar(str_i, c);
                        str_i++;
                    }
                }
            }
            else {
                str->string[str_i] = '\0';
                CurrentBlock->sections[sec_i].addAttribute(str2->string, str->string);
                delete str;
                delete str2;
                str = new String();
                str2 = new String();
                str_i = 0;
                str2_i = 0;
                if (c == ';') {
                    mode = ParseProperty;

                }
                else if (c == '}') {
                    mode = ParseSelector;
                    sec_i++;
                }

            }
        }
        else if (mode == Command) {
            if (c == '\n') {

            }
            else {
                if (str->length + 1 < str->size) {
                    str->addChar(str_i, c);
                    str_i++;
                }
                else {
                    str->expandString();
                    str->addChar(str_i, c);
                    str_i++;
                }
            }

            if (c == ',') {
                if (com1_i < 0) {
                    com1_i = str_i - 1;
                }
                else if (com2_i < 0) {
                    com2_i = str_i - 1;
                }
                
            }



            if (str_i > 3 && str->string[0] == '*' && str->string[1] == '*' && str->string[2] == '*' && str->string[3] == '*')
            {
                mode = ParseSelector;
                str_i = 0;
                delete str;
                str = new String();
            }

            else {
                if (str_i > 0 && isspace(c) && com1_i > 0 && com2_i > 0 || str->string[str_i - 1] == '?') {

                    if (str->string[0] == '?') {
                        if (BlockList != nullptr)
                            cout << "? == " << countSections(BlockList) << endl;
                        else
                            cout << "? == 0" << endl;
                        str_i = 0;
                        delete str;
                        str = new String();
                        com1_i = -1;
                        com2_i = -1;
                    }


                    str->string[str_i] = '\0';
                    if (str->string[com1_i + 1] == 'S') //selector
                    {
                        if (str->string[com2_i + 1] == '?') {
                            if (isdigit(str->string[0])) { //wypisz liczbę selektorów dla sekcji nr i (numery zaczynają się od 1), jeśli nie ma pomiń;
                                i = toInt(str->string, 0, com1_i - 1);
                                if (findSection(BlockList, i) != nullptr)
                                    cout << str->string << " == " << countSelectors(findSection(BlockList, i)) << endl;



                            }
                            else { //wypisz łączną (dla wszystkich bloków) liczbę wystąpień selektora z. Możliwe jest 0;
                                z = new char[com1_i];
                                strcpy(z, str->string);
                                z[com1_i] = '\0';
                                cout << str->string << " == " << countSelector(BlockList, z) << endl;

                            }
                        }
                        else { //wypisz j-ty selector dla i-tego bloku (numery sekcji oraz atrybutów zaczynają się od 1) jeśli nie ma sekcji lub selektora pomiń;
                            i = toInt(str->string, 0, com1_i - 1);
                            j = toInt(str->string, com2_i + 1, str->length - 1);
                            if (findSection(BlockList, i) != nullptr && findSection(BlockList, i)->findSelector(j) != nullptr)
                                cout << str->string << " == " << findSection(BlockList, i)->findSelector(j)->name << endl;


                        }
                    }
                    else if (str->string[com1_i + 1] == 'A') //attribute
                    {
                        if (str->string[com2_i + 1] == '?') {
                            if (isdigit(str->string[0])) { //wypisz liczbę atrybutów dla sekcji nr i, jeśli nie ma takiego bloku lub sekcji pomiń;
                                i = toInt(str->string, 0, com1_i - 1);
                                if (findSection(BlockList, i) != nullptr)
                                    cout << str->string << " == " << countAttributes(findSection(BlockList, i)) << endl;
                            }
                            else { //wypisz łączną liczbę wystąpień atrybutu nazwie n. Możliwe jest 0
                                z = new char[com1_i];
                                strcpy(z, str->string);
                                z[com1_i] = '\0';
                                cout << str->string << " == " << countAttribute(BlockList, z) << endl;

                            }
                        }
                        else { //wypisz dla i-tej sekcji wartość atrybutu o nazwie n, jeśli nie ma takiego pomiń;
                            i = toInt(str->string, 0, com1_i - 1);
                            z = new char[str->length - com2_i - 1];
                            strcpy(z, &(str->string[com2_i + 1]));
                            z[str->length - com2_i - 1] = '\0';
                            if (findSection(BlockList, i) != nullptr && findSection(BlockList, i)->findAttributeByProperty(z) != nullptr)
                                cout << str->string << " == " << findSection(BlockList, i)->findAttributeByProperty(z)->value << endl;

                        }
                    }
                    else if (str->string[com1_i + 1] == 'D')  //delete
                    {
                        if (str->string[com2_i + 1] == '*') { //usuń całą sekcję nr i (tj. separatory+atrybuty), po poprawnym wykonaniu wypisz deleted;
                            if (isdigit(str->string[0])) {
                                i = toInt(str->string, 0, com1_i - 1);
                                if (findSection(BlockList, i) != nullptr && removeSection(BlockList, findSection(BlockList, i)))
                                    cout << str->string << " == deleted" << endl;

                            }
                        }
                        else { //usuń z i-tej sekcji atrybut o nazwie n, jeśli pozostaje pusta sekcja powinna zostać usunięta (wraz z ew. selektorami),wypisz deleted.
                            if (isdigit(str->string[0])) {
                                i = toInt(str->string, 0, com1_i - 1);
                                z = new char[str->length - com2_i - 1];
                                strcpy(z, &(str->string[com2_i + 1]));
                                z[str->length - com2_i - 1] = '\0';
                                if (findSection(BlockList, i) != nullptr && removeAttribute(BlockList, findSection(BlockList, i), z))
                                    cout << str->string << " == deleted" << endl;

                            }
                        }
                    }
                    else if (str->string[com1_i + 1] == 'E') //wypisz wartość atrybutu o nazwie n dla selektora z, bierzemy ostatnie. W przypadku braku pomiń;
                    {
                        z = new char[com1_i];
                        strcpy(z, str->string);
                        z[com1_i] = '\0';
                        n = new char[str->length - com2_i - 1];
                        strcpy(n, &(str->string[com2_i + 1]));
                        n[str->length - com2_i - 1] = '\0';
                        if (findNewestSelector(BlockList, z) != nullptr && findNewestSelector(BlockList, z)->findAttributeByProperty(n) != nullptr)
                            cout << str->string << " == " << findNewestSelector(BlockList, z)->findAttributeByProperty(n)->value << endl;

                    }

                    str_i = 0;
                    delete str;
                    str = new String();
                    com1_i = -1;
                    com2_i = -1;

                }
                else { //bledna komenda lub wczytujemy dalej

                }
            }

        }

    }
    freeBlockList(BlockList);
    delete str;
    delete str2;

    return 0;
}

