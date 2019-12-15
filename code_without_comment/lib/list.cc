template <class T>
ListElement<T>::ListElement(T itm)
{
     item = itm;
     next = NULL;
}

template <class T>
List<T>::List()
{
    first = last = NULL;
    numInList = 0;
}

template <class T>
List<T>::~List()
{
}

template <class T>
void List<T>::Append(T item)
{
    ListElement<T> *element = new ListElement<T>(item);

    ASSERT(!IsInList(item));

    if (IsEmpty()) {
        first = element;
        last = element;
    }
    else {
        last->next = element;
        last = element;
    }
    numInList++;
    ASSERT(IsInList(item));
}

template <class T>
void List<T>::Prepend(T item)
{
    ListElement<T> *element = new ListElement<T>(item);

    ASSERT(!IsInList(item));
    if (IsEmpty()) {
        first = element;
        last = element;
    }
    else {
        element->next = first;
        first = element;
    }
    numInList++;
    ASSERT(IsInList(item));
}

template <class T>
T List<T>::RemoveFront()
{
    ListElement<T> *element = first;
    T thing;

    ASSERT(!IsEmpty());

    thing = first->item;
    if (first == last) {
        first = NULL;
		last = NULL;
    }
    else {
        first = element->next;
    }
    numInList--;
    delete element;
    return thing;
}

template <class T>
void List<T>::Remove(T item)
{
    ListElement<T> *prev, *ptr;
    T removed;

    ASSERT(IsInList(item));

    if (item == first->item) {
        removed = RemoveFront();
        ASSERT(item == removed);
    }
    else {
        prev = first;
        for (ptr = first->next; ptr != NULL; prev = ptr, ptr = ptr->next) {
            if (item == ptr->item) {
                prev->next = ptr->next;
                if (prev->next == NULL) {
                    last = prev;
                }
                delete ptr;
                numInList--;
                break;
            }
        }
        ASSERT(ptr != NULL);
    }
   ASSERT(!IsInList(item));
}

template <class T>
bool List<T>::IsInList(T item) const
{
    ListElement<T> *ptr;

    for (ptr = first; ptr != NULL; ptr = ptr->next) {
        if (item == ptr->item) {
            return TRUE;
        }
    }
    return FALSE;
}

template <class T>
void List<T>::Apply(void (*func)(T)) const
{
    ListElement<T> *ptr;

    for (ptr = first; ptr != NULL; ptr = ptr->next) {
        (*func)(ptr->item);
    }
}

template <class T>
void SortedList<T>::Insert(T item)
{
    ListElement<T> *element = new ListElement<T>(item);
    ListElement<T> *ptr;

    ASSERT(!IsInList(item));
    if (this->IsEmpty()) {
        this->first = element;
        this->last = element;
    }
    else if (compare(item, this->first->item) < 0) {
        element->next = this->first;
        this->first = element;
    }
    else {
        for (ptr = this->first; ptr->next != NULL; ptr = ptr->next) {
            if (compare(item, ptr->next->item) < 0) {
                element->next = ptr->next;
                ptr->next = element;
                this->numInList++;
                return;
            }
        }
        this->last->next = element;
        this->last = element;
    }
    this->numInList++;
    ASSERT(IsInList(item));
}

template <class T>
void List<T>::SanityCheck() const
{
    ListElement<T> *ptr;
    int numFound;

    if (first == NULL) {
        ASSERT((numInList == 0) && (last == NULL));
    }
    else if (first == last) {
        ASSERT((numInList == 1) && (last->next == NULL));
    }
    else {
        for (numFound = 1, ptr = first; ptr != last; ptr = ptr->next) {
            numFound++;
            ASSERT(numFound <= numInList);
        }
        ASSERT(numFound == numInList);
        ASSERT(last->next == NULL);
    }
}

template <class T>
void List<T>::SelfTest(T *p, int numEntries)
{
    int i;
    ListIterator<T> *iterator = new ListIterator<T>(this);

    SanityCheck();

    ASSERT(IsEmpty() && (first == NULL));
    for (; !iterator->IsDone(); iterator->Next()) {
        ASSERTNOTREACHED();
    }

    for (i = 0; i < numEntries; i++) {
        Append(p[i]);
        ASSERT(IsInList(p[i]));
        ASSERT(!IsEmpty());
     }
     SanityCheck();

     for (i = 0; i < numEntries; i++) {
        Remove(p[i]);
        ASSERT(!IsInList(p[i]));
     }
     ASSERT(IsEmpty());
     SanityCheck();
     delete iterator;
}

template <class T>
void SortedList<T>::SanityCheck() const
{
    ListElement<T> *prev, *ptr;

    List<T>::SanityCheck();
    if (this->first != this->last) {
        for (prev = this->first, ptr = this->first->next; ptr != NULL; prev = ptr, ptr = ptr->next) {
            ASSERT(compare(prev->item, ptr->item) <= 0);
        }
    }
}

template <class T>
void SortedList<T>::SelfTest(T *p, int numEntries)
{
    int i;
    T *q = new T[numEntries];

    List<T>::SelfTest(p, numEntries);

    for (i = 0; i < numEntries; i++) {
        Insert(p[i]);
        ASSERT(IsInList(p[i]));
     }
     SanityCheck();

     for (i = 0; i < numEntries; i++) {
        q[i] = this->RemoveFront();
        ASSERT(!IsInList(q[i]));
     }
     ASSERT(this->IsEmpty());

     for (i = 0; i < (numEntries - 1); i++) {
        ASSERT(compare(q[i], q[i + 1]) <= 0);
     }
     SanityCheck();

     delete q;
}
