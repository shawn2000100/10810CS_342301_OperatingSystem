#include "list.cc"

template <class T>
class ListElement {
  public:
    ListElement(T itm);
    ListElement *next;
    T item;
};

template <class T> class ListIterator;

template <class T>
class List {
  public:
    List();
    virtual ~List();

    virtual void Prepend(T item);
    virtual void Append(T item);

    T Front() { return first->item; }

    T RemoveFront();
    void Remove(T item);

    bool IsInList(T item) const;

    unsigned int NumInList() { return numInList;};

    bool IsEmpty() { return (numInList == 0); };

    void Apply(void (*f)(T)) const;

    virtual void SanityCheck() const;

    void SelfTest(T *p, int numEntries);

  protected:
    ListElement<T> *first;
    ListElement<T> *last;
    int numInList;

    friend class ListIterator<T>;
};

template <class T>
class SortedList : public List<T> {
  public:
    SortedList(int (*comp)(T x, T y)) : List<T>() { compare = comp;};
    ~SortedList() {};

    void Insert(T item);

    void SanityCheck() const;
    void SelfTest(T *p, int numEntries);

  private:
    int (*compare)(T x, T y);

    void Prepend(T item) { Insert(item); }

    void Append(T item) { Insert(item); }
};

template <class T>
class ListIterator {
  public:
    ListIterator(List<T> *list) { current = list->first; }

    bool IsDone() { return current == NULL; };

    T Item() { ASSERT(!IsDone()); return current->item; };

    void Next() { current = current->next; };

  private:
    ListElement<T> *current;
};
