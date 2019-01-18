#ifndef LAB4_IBSTREE_HPP
#define LAB4_IBSTREE_HPP

#include <set>
using std::set;

#include <vector>
using std::vector;

#include <functional>
using std::function;

#include <type_traits>
using std::is_copy_constructible;

/**
 * Шаблонный интерфейс бинарного дерева поиска
 * @tparam K Тип ключа
 * @tparam V Тип значения
 */
template <class K, class V>
class IBSTree {
public:
    /**
     * Добавляет переданное значение с переданным значением ключа в дерево
     * @param key Значение ключа
     * @param value Значение элемента
     */
    virtual void put(K key, V value) = 0;

    /**
     * Удаляет из дерева элемент с переданным значением ключа
     * @param key Значение ключа
     */
    virtual void remove(K key) = 0;

    /**
     *     Возвращает значение элемента в дереве с переданным значением ключа,
     * или возвращает переданное стандартное значение элемента
     * @param key Значение ключа
     * @param defaultValue Стандартное значение элемента
     * @return Искомое значение элемента, если есть элемент с таким ключом или стандартное значение
     */
    virtual V getOrDefault(K key, V defaultValue) = 0;

    /**
     * Возвращает множество всех значений ключей, содержащихся в дереве
     * @return множество всех значений ключей, содержащихся в дереве
     */
    virtual set<K> keys() = 0;

    /**
     * Возвращает массив всех значений элементов, содержащихся в дереве
     * @return Массив всех значений элементов, содержащихся в дереве
     */
    virtual vector<V> values() = 0;

    /**
     * Возвращает количество пар (ключ, значение) содержащихся в дереве
     * @return Количество пар (ключ, значение) содержащихся в дереве
     */
    virtual int size() = 0;

    /**
     * Сообщает, пусто ли дерево
     * @return true, если пусто, иначе false
     */
    virtual bool empty() = 0;

    /**
     * Инфиксный обход (лкп) по всем элементам дерева с применением к каждой паре (ключ, значение) передаваемой функции
     * @param consume Функция для обхода
     */
    virtual void foreach_infix  (function<void(K,V)> consume) = 0;

    /**
     * Префиксный обход (клп) по всем элементам дерева с применением к каждой паре (ключ, значение) передаваемой функции
     * @param consume Функция для обхода
     */
    virtual void foreach_prefix (function<void(K,V)> consume) = 0;

    /**
     * Пост обход (лпк) по всем элементам дерева с применением к каждой паре (ключ, значение) передаваемой функции
     * @param consume Функция для обхода
     */
    virtual void foreach_postfix(function<void(K,V)> consume) = 0;
};

#endif //LAB4_IBSTREE_HPP
