#pragma once
#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iterator>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value{};
        Node* next_node{};
    };

    //итератор
    template <typename ValueType>
    class BasicIterator {
    private:
        //template <typename T> если итератор надо подружить с любыми типами SingleLinkedList<T>
        friend class SingleLinkedList;
        //здесь надо конструктор, в который передаётся указатель на приватную структуру Note
        BasicIterator(Node* node) : node_{ node } {}

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator ==(const BasicIterator<const Type>& rhs)const noexcept {
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator !=(const BasicIterator<const Type>& rhs)const noexcept {
            return node_ != rhs.node_;
        }
        [[nodiscard]] bool operator ==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator !=(const BasicIterator<Type>& rhs) const noexcept {
            return node_ != rhs.node_;
        }

        BasicIterator& operator++() noexcept {  //перегрузка префиксного оператора ++
            //изменяет текущий объект
            node_ = node_->next_node;
            return *this;   //возвращает ссылку на этот объект
        }
        BasicIterator operator++(int) noexcept {    //перегрузка постфиксного оператора ++
            auto old_value(*this);   // Сохраняем прежнее значение объекта для последующего возврата
            ++(*this);  // используем логику префиксной формы инкремента
            return old_value;
        }

        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }
        [[nodiscard]] pointer operator->() const noexcept {
            return &node_->value;
        }
    private:
        Node* node_{};
    };

    void PushBack(const Type& value) {
        Node* current = &head_;
        while (current->next_node != nullptr) {
            current = current->next_node;
        }
        current->next_node = new Node(value, nullptr);
        ++size_;
    }

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    SingleLinkedList() = default;

    ~SingleLinkedList() {
        Clear();
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        for (auto it = values.begin(); it != values.end(); ++it) {
            PushBack(*it);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        for (auto it = other.begin(); it != other.end(); ++it) {
            PushBack(*it);
        }
    }

    SingleLinkedList& operator =(const SingleLinkedList& other) {
        if (this != &other) {
            //сначала заполнить этот
            SingleLinkedList temp_list(other);
            //и свапнуть с temp_list
            this->swap(temp_list);
            //temp_list при выходе из оператора присваивания вызовет свой деструктор 
        }
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        size_t temp_size = other.size_;
        other.size_ = size_;
        size_ = temp_size;

        Node* temp_head = other.head_.next_node;
        other.head_.next_node = head_.next_node;
        head_.next_node = temp_head;
    }

    void PushFront(const Type& value) { //добавление в начало
        //в head записываем новый Node, созданный через new
        //он получит следующий Node из head, и value
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() {
        //повторять пока head.next_node не равен nullptr
        while (head_.next_node != nullptr) {
            Node* current_node = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete current_node;
        }
        size_ = 0;
    }

    // Возвращает количество элементов в списке
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Сообщает, пустой ли список
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    /// <summary>
    /// Итераторы
    /// </summary>

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }
    // Константные версии begin/end для обхода списка без возможности модификации его элементов
    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator(head_.next_node);
    }
    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator(nullptr);
    }
    // Методы для удобного получения константных итераторов у неконстантного контейнера
    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return begin();
    }
    [[nodiscard]] ConstIterator cend() const noexcept {
        return end();
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return before_begin();
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{ const_cast<Node*>(&head_) };
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* node_to_insert = new Node(value, pos.node_->next_node);
        pos.node_->next_node = node_to_insert;
        ++size_;
        return node_to_insert;
    }

    void PopFront() noexcept {
        if (size_ > 0) {
            Node* to_delete = head_.next_node;
            head_.next_node = to_delete->next_node;
            delete to_delete;
        }
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node* to_erase = pos.node_->next_node;
        Node* after_erase = to_erase->next_node;
        delete to_erase;
        pos.node_->next_node = after_erase;
        return Iterator{ after_erase };
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_{};
    size_t size_{};
};



template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator ==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs) && !(rhs < lhs);
}

template <typename Type>
bool operator !=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator <(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator <=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator >(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator >=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}