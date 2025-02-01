#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <queue>
#include <stack>
using namespace std;

struct Book
{
    int bookID;
    string title;
    int year;
    int quantity;
    Book *next;
    Book(int id, const string &title, int year, int quantity)
        : bookID(id), title(title), year(year), quantity(quantity), next(nullptr) {}
};

struct BorrowedBook
{
    int bookID;
    string title;
    time_t expiryDate;

    BorrowedBook(int id, const string &title, time_t expiryDate)
        : bookID(id), title(title), expiryDate(expiryDate) {}
};

class Library
{
private:
    Book *head;
    queue<BorrowedBook> borrowedQueue;
    stack< pair < string, Book > > actionHistory;

    Book *split(Book *source)
    {
        Book *slow = source;
        Book *fast = source->next;

        while (fast && fast->next)
        {
            slow = slow->next;
            fast = fast->next->next;
        }
        Book *mid = slow->next;
        slow->next = nullptr;
        return mid;
    }

    Book *merge(Book *left, Book *right, int sortOption)
    {
        if (!left)
            return right;
        if (!right)
            return left;

        bool condition;

        if (sortOption == 1)
        {
            condition = (left->title < right->title); // Sorting by Title
        }
        else if (sortOption == 2)
        {
            condition = (left->year < right->year); // Sorting by Year
        }
        else if (sortOption == 3)
        {
            condition = (left->bookID < right->bookID); // Sorting by BookID
        }
        else
        {
            condition = (left->title < right->title); // If invalid sortOption, default to sorting by Title
        }

        if (condition)
        {
            left->next = merge(left->next, right, sortOption);
            return left;
        }
        else
        {
            right->next = merge(left, right->next, sortOption);
            return right;
        }
    }

    Book *mergeSort(Book *node, int sortOption)
    {
        if (!node || !node->next)
            return node;

        Book *mid = split(node);
        Book *left = mergeSort(node, sortOption);
        Book *right = mergeSort(mid, sortOption);

        return merge(left, right, sortOption);
    }

public:
    Library() : head(nullptr) {}

    void addBook(int id, const string &title, int year, int quantity)
    {
        Book *current = head;

        while (current != nullptr)
        {
            if (current->bookID == id && current->title == title)
            {
                current->quantity += quantity;
                cout << "Updated quantity for existing book: " << title << " (ID: " << id << "), New Quantity: " << current->quantity << endl;
                return;
            }
            current = current->next;
        }

        Book *newBook = new Book(id, title, year, quantity);
        newBook->next = head;
        head = newBook;
        actionHistory.push(make_pair("add", *newBook));

        cout << "Book added successfully: " << title << endl;
    }


    // Linked List Deletion
    void removeBook(int bookID)
    {
        Book *current = head;
        Book *prev = nullptr;

        while (current)
        {
            if (current->bookID == bookID)
            {
                if (prev)
                {
                    prev->next = current->next;
                }
                else
                {
                    head = current->next;
                }

                actionHistory.push(make_pair("remove", *current));

                delete current;
                cout << "Book removed successfully." << endl;
                return;
            }
            prev = current;
            current = current->next;
        }
        cout << "No book found with ID " << bookID << endl;
    }

    void sortBooks(int sortOption)
    {
        if (!head || !head->next)
        {
            cout << "Sorting not required for an empty or single book list." << endl;
            return;
        }

        head = mergeSort(head, sortOption);
        cout << "Books sorted successfully." << endl;
        displayBooks();
    }

    // Use of STACK
    void undo()
    {
        if (actionHistory.empty())
        {
            cout << "No actions to undo." << endl;
            return;
        }

        auto lastAction = actionHistory.top();
        actionHistory.pop();

        if (lastAction.first == "add")
        {
            removeBook(lastAction.second.bookID);
        }
        else if (lastAction.first == "remove")
        {
            addBook(lastAction.second.bookID, lastAction.second.title, lastAction.second.year, lastAction.second.quantity);
        }
        cout << "Last action undone." << endl;
    }

    // Linked List traversal
    void searchBook(const string &query) const
    {
        Book *current = head;
        bool found = false;

        while (current)
        {
            if (current->title == query)
            {
                cout << "\nBook Found: " << endl;
                cout << "ID: " << current->bookID
                     << ", Title: " << current->title
                     << ", Year: " << current->year
                     << ", Quantity: " << current->quantity << endl;
                found = true;
            }
            current = current->next;
        }

        if (!found)
        {
            cout << "No book found with the title \"" << query << "\"." << endl;
        }
    }

    void searchBookWithID(const int &b_id) const
    {
        Book *current = head;
        bool found = false;

        while (current)
        {
            if (current->bookID == b_id)
            {
                cout << "\nBook Found: " << endl;
                cout << "ID: " << current->bookID
                     << ", Title: " << current->title
                     << ", Year: " << current->year
                     << ", Quantity: " << current->quantity << endl;
                found = true;
            }
            current = current->next;
        }

        if (!found)
        {
            cout << "No book found with the Book ID \"" << b_id << "\"." << endl;
        }
    }

    void borrowBookWithExpiry(int bookID)
    {
        Book *current = head;
        time_t now = time(0);
        const int borrowPeriodDays = 30;
        time_t expiryDate = now + (borrowPeriodDays * 24 * 60 * 60);

        while (current)
        {
            if (current->bookID == bookID)
            {
                if (current->quantity > 0)
                {
                    current->quantity--;
                    borrowedQueue.push(BorrowedBook(bookID, current->title, expiryDate));
                    cout << "Book borrowed successfully: " << current->title
                         << ". Return by: " << ctime(&expiryDate) << endl;
                    return;
                }
                else
                {
                    cout << "Sorry, the book is currently unavailable." << endl;
                    return;
                }
            }
            current = current->next;
        }
        cout << "No book found with ID " << bookID << endl;
    }

    void checkExpiryDates()
    {
        if (borrowedQueue.empty())
        {
            cout << "No borrowed books in the queue." << endl;
            return;
        }

        cout << "\nBooks nearing expiry:" << endl;
        cout << setw(5) << "ID" << setw(20) << "Title" << setw(30) << "Expiry Date" << endl;
        cout << string(55, '-') << endl;

        time_t now = time(0);
        int count = 0;

        queue<BorrowedBook> tempQueue;
        while (!borrowedQueue.empty())
        {
            BorrowedBook borrowed = borrowedQueue.front();
            borrowedQueue.pop();

            if (borrowed.expiryDate < now)
            {
                cout << setw(5) << borrowed.bookID
                     << setw(20) << borrowed.title
                     << setw(30) << "Expired" << endl;
            }
            else
            {
                tempQueue.push(borrowed);
                count++;
                cout << setw(5) << borrowed.bookID
                     << setw(20) << borrowed.title
                     << setw(30) << ctime(&borrowed.expiryDate);
            }
        }
        borrowedQueue = tempQueue;

        if (count == 0)
            cout << "No books are nearing expiry." << endl;
    }

    // Use of Queue
    void returnBook(int bookID)
    {
        Book *current = head;

        while (current)
        {
            if (current->bookID == bookID)
            {
                current->quantity++;
                cout << "Book returned successfully: " << current->title << endl;

                queue<BorrowedBook> tempQueue;
                bool found = false;

                while (!borrowedQueue.empty())
                {
                    BorrowedBook borrowed = borrowedQueue.front();
                    borrowedQueue.pop();

                    if (borrowed.bookID == bookID)
                    {
                        found = true;
                        cout << "Returned book removed from borrowed list." << endl;
                    }
                    else
                    {
                        tempQueue.push(borrowed);
                    }
                }
                borrowedQueue = tempQueue;

                if (!found)
                {
                    cout << "Book not found in borrowed list." << endl;
                }

                return;
            }
            current = current->next;
        }
        cout << "No book found with ID " << bookID << endl;
    }

    void displayBooks() const
    {
        if (!head)
        {
            cout << "The library is empty." << endl;
            return;
        }

        cout << "\nLibrary Collection:" << endl;
        cout << setw(5) << "ID" << setw(20) << "Title" << setw(10) << "Year" << setw(10) << "Quantity" << endl;
        cout << string(50, '-') << endl;

        Book *current = head;
        while (current)
        {
            cout << setw(5) << current->bookID
                 << setw(20) << current->title
                 << setw(10) << current->year
                 << setw(10) << current->quantity << endl;
            current = current->next;
        }
    }

    ~Library()
    {
        while (head)
        {
            Book *temp = head;
            head = head->next;
            delete temp;
        }
    }
};

void showRoleMenu()
{
    cout << "\nSelect your role: ";
    cout << "\n1. Admin Panel";
    cout << "\n2. Student Panel";
    cout << "\n3. Exit";
    cout << "\nEnter your choice: ";
}

void studentPanel(Library &library)
{
    int choice;
    do
    {
        cout << "\nStudent Panel";
        cout << "\n1. Search Book";
        cout << "\n2. Display All Books";
        cout << "\n3. Sort Books ";
        cout << "\n4. Borrow Book";
        cout << "\n5. Return Book";
        cout << "\n6. Check Expiry Dates";
        cout << "\n7. Exit";
        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string query;
            cout << "Enter book title to search: ";
            cin.ignore();
            getline(cin, query);
            library.searchBook(query);
            break;
        }
        case 2:
        {
            library.displayBooks();
            break;
        }

        case 3:
        {
            int sortOption;
            cout << "Sort by (1: Title 2: Year 3: Book ID): ";
            cin >> sortOption;
            library.sortBooks(sortOption);
            break;
        }

        case 4:
        {
            int bookID;
            cout << "Enter book ID to borrow: ";
            cin >> bookID;
            library.borrowBookWithExpiry(bookID);
            break;
        }
        case 5:
        {
            int bookID;
            cout << "Enter book ID to return: ";
            cin >> bookID;
            library.returnBook(bookID);
            break;
        }
        case 6:
            library.checkExpiryDates();
            break;
        case 7:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice. Try again." << endl;
        }
    } while (choice != 7);
}

int main()
{
    Library library;
    int roleChoice;

    do
    {
        showRoleMenu();
        cin >> roleChoice;

        if (roleChoice == 1)
        {
            int choice;
            do
            {
                cout << "\nAdmin Panel";
                cout << "\n1. Add Book";
                cout << "\n2. Remove Book";
                cout << "\n3. Display All Books";
                cout << "\n4. Search Book ";
                cout << "\n5. Sort Books ";
                cout << "\n6. Undo Last Action";
                cout << "\n7. Exit";
                cout << "\nEnter your choice: ";
                cin >> choice;

                switch (choice)
                {
                case 1:
                {
                    int id, year, quantity;
                    string title;
                    cout << "Enter book ID: ";
                    cin >> id;
                    cout << "Enter book title: ";
                    cin.ignore();
                    getline(cin, title);
                    cout << "Enter book year: ";
                    cin >> year;
                    cout << "Enter book quantity: ";
                    cin >> quantity;
                    library.addBook(id, title, year, quantity);
                    break;
                }
                case 2:
                {
                    int bookID;
                    cout << "Enter book ID to remove: ";
                    cin >> bookID;
                    library.removeBook(bookID);
                    break;
                }
                case 3:
                    library.displayBooks();
                    break;

                case 4:
                {
                    int searchChoice;
                    cout << "Search by (1: Title 2: Book ID): ";
                    cin >> searchChoice;
                    if (searchChoice == 1)
                    {
                        string query;
                        cout << "Enter book title to search: ";
                        cin.ignore();
                        getline(cin, query);
                        library.searchBook(query);
                        break;
                    }
                    else if (searchChoice == 2)
                    {
                        int boiID;
                        cout << "Enter book id to search: ";
                        cin >> boiID;
                        library.searchBookWithID(boiID);
                        break;
                    }
                    else
                    {
                        cout << "Invalid choice \n";
                    }
                    break;
                }
                case 5:
                {
                    int sortOption;
                    cout << "Sort by (1: Title 2: Year 3: Book ID): ";
                    cin >> sortOption;
                    library.sortBooks(sortOption);
                    break;
                }
                case 6:
                    library.undo();
                    break;
                case 7:
                    cout << "Exiting..." << endl;
                    break;
                default:
                    cout << "Invalid choice. Try again." << endl;
                }
            } while (choice != 7);
        }
        else if (roleChoice == 2)
        {
            studentPanel(library);
        }
    } while (roleChoice != 3);

    return 0;
}
