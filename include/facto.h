
#include <array>
#include <cstddef>
#include <utility>

template<typename T, std::size_t Num>
class Factory {
public:
    // Construction des Num instances avec les mêmes arguments
    template<typename... Args>
    explicit Factory(Args&&... args)
        : instances{
            (static_cast<void>(std::size_t{}), T(args...))...
        }
    {}

    // Accès par ID
    T& get_instance(std::size_t id) {
        return instances.at(id);
    }

    const T& get_instance(std::size_t id) const {
        return instances.at(id);
    }

    // Accès avec []
    T& operator[](std::size_t id) {
        return instances.at(id);
    }

    const T& operator[](std::size_t id) const {
        return instances.at(id);
    }

    // Parcours de toutes les instances
    template<typename Func>
    void for_each(Func&& func) {
        for (std::size_t i = 0; i < Num; ++i) {
            func(instances[i], i);
        }
    }

    // Nombre d'instances
    static constexpr std::size_t count() {
        return Num;
    }

private:
    std::array<T, Num> instances;
};
