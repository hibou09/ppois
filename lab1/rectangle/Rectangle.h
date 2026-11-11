/**
 * @file Rectangle.h
 * @brief Класс прямоугольника со сторонами, параллельными осям координат.
 * @details Вершины имеют целочисленные координаты. Класс не зависит от
 *          пользовательского интерфейса: ввод и вывод выполняются через
 *          произвольные потоки (std::istream / std::ostream).
 */
#ifndef LAB_RECTANGLE_H
#define LAB_RECTANGLE_H

#include <iosfwd>

namespace lab {

/**
 * @brief Точка плоскости с целочисленными координатами.
 */
struct Point {
    int x; ///< координата по оси X
    int y; ///< координата по оси Y
};

/**
 * @brief Сравнение точек.
 * @param a первая точка
 * @param b вторая точка
 * @return true, если обе координаты совпадают
 */
bool operator==(const Point& a, const Point& b);

/// @brief Точки не равны (см. operator==(const Point&, const Point&)).
bool operator!=(const Point& a, const Point& b);

/**
 * @brief Прямоугольник, стороны которого параллельны осям координат.
 * @details Хранятся левая нижняя (left, bottom) и правая верхняя (right, top)
 *          вершины. Инвариант класса: left <= right и bottom <= top.
 *          Допускаются вырожденные прямоугольники (нулевая ширина/высота).
 *          Копирование и присваивание поэлементные (правило нуля), поэтому
 *          деструктор не требуется.
 */
class Rectangle {
public:
    /// @brief Создаёт вырожденный прямоугольник (0,0)-(0,0).
    Rectangle();

    /**
     * @brief Создаёт прямоугольник по двум противоположным вершинам.
     * @details Порядок вершин не важен: координаты нормализуются.
     * @param x1 X первой вершины
     * @param y1 Y первой вершины
     * @param x2 X противоположной вершины
     * @param y2 Y противоположной вершины
     */
    Rectangle(int x1, int y1, int x2, int y2);

    /// @brief Конструктор копирования.
    Rectangle(const Rectangle& other) = default;

    /// @brief Оператор присваивания.
    Rectangle& operator=(const Rectangle& other) = default;

    /**
     * @brief Статический фабричный метод: прямоугольник по углу и размерам.
     * @param x X левой нижней вершины
     * @param y Y левой нижней вершины
     * @param width ширина (>= 0)
     * @param height высота (>= 0)
     * @return новый прямоугольник
     * @throw std::invalid_argument если width или height отрицательны
     */
    static Rectangle fromSize(int x, int y, int width, int height);

    /// @name Получение координат
    /// @{
    int left() const;       ///< @return X левой стороны
    int right() const;      ///< @return X правой стороны
    int bottom() const;     ///< @return Y нижней стороны
    int top() const;        ///< @return Y верхней стороны
    Point bottomLeft() const;  ///< @return левая нижняя вершина
    Point bottomRight() const; ///< @return правая нижняя вершина
    Point topLeft() const;     ///< @return левая верхняя вершина
    Point topRight() const;    ///< @return правая верхняя вершина
    int width() const;         ///< @return ширина
    int height() const;        ///< @return высота
    long long area() const;    ///< @return площадь
    /// @}

    /**
     * @brief Перемещает прямоугольник на вектор (dx, dy).
     * @param dx сдвиг по X
     * @param dy сдвиг по Y
     * @return ссылка на себя
     * @see moveTo()
     */
    Rectangle& move(int dx, int dy);

    /**
     * @brief Перемещает левую нижнюю вершину в точку (x, y).
     * @return ссылка на себя
     * @see move()
     */
    Rectangle& moveTo(int x, int y);

    /**
     * @brief Изменяет размер; левая нижняя вершина остаётся на месте.
     * @param newWidth новая ширина (>= 0)
     * @param newHeight новая высота (>= 0)
     * @return ссылка на себя
     * @throw std::invalid_argument если размер отрицателен
     */
    Rectangle& resize(int newWidth, int newHeight);

    /**
     * @brief Префиксный инкремент: ширина и высота +1.
     * @details Правая верхняя вершина сдвигается на (1, 1).
     * @return ссылка на изменённый объект
     */
    Rectangle& operator++();

    /// @brief Постфиксный инкремент. @return копия до изменения
    Rectangle operator++(int);

    /**
     * @brief Префиксный декремент: ширина и высота -1.
     * @return ссылка на изменённый объект
     * @throw std::domain_error если ширина или высота уже равны нулю
     *        (объект при этом не изменяется)
     */
    Rectangle& operator--();

    /// @brief Постфиксный декремент. @return копия до изменения
    /// @throw std::domain_error см. operator--()
    Rectangle operator--(int);

    /**
     * @brief Заменяет *this наименьшим прямоугольником, содержащим оба.
     * @param other второй прямоугольник
     * @return ссылка на себя
     */
    Rectangle& operator+=(const Rectangle& other);

    /**
     * @brief Заменяет *this пересечением двух прямоугольников.
     * @details Если общей части нет, результат — вырожденный
     *          прямоугольник (0,0)-(0,0). Проверить наличие общей части
     *          заранее можно методом intersects().
     * @param other второй прямоугольник
     * @return ссылка на себя
     */
    Rectangle& operator-=(const Rectangle& other);

    /**
     * @brief Есть ли общая часть (включая касание границ).
     * @param other второй прямоугольник
     * @return true, если пересечение непусто
     */
    bool intersects(const Rectangle& other) const;

private:
    int left_;   ///< X левой стороны
    int bottom_; ///< Y нижней стороны
    int right_;  ///< X правой стороны
    int top_;    ///< Y верхней стороны
};

/**
 * @brief Наименьший прямоугольник, содержащий оба.
 * @return новый прямоугольник
 * @see Rectangle::operator+=
 */
Rectangle operator+(Rectangle lhs, const Rectangle& rhs);

/**
 * @brief Пересечение двух прямоугольников.
 * @return новый прямоугольник
 * @see Rectangle::operator-=
 */
Rectangle operator-(Rectangle lhs, const Rectangle& rhs);

/// @brief Равенство: совпадают все координаты.
bool operator==(const Rectangle& a, const Rectangle& b);
/// @brief Неравенство.
bool operator!=(const Rectangle& a, const Rectangle& b);

/**
 * @brief Вывод в поток в формате "(l, b)-(r, t)".
 * @param out поток вывода
 * @param r прямоугольник
 * @return out
 */
std::ostream& operator<<(std::ostream& out, const Rectangle& r);

/**
 * @brief Ввод из потока: четыре целых "x1 y1 x2 y2".
 * @details При ошибке чтения объект не меняется, у потока выставляется failbit.
 * @param in поток ввода
 * @param r куда прочитать
 * @return in
 */
std::istream& operator>>(std::istream& in, Rectangle& r);

} // namespace lab

#endif // LAB_RECTANGLE_H
