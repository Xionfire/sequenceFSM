#include <array>
#include <cstddef>
#include <utility>
#include <tuple>

// Utilitaire pour construire un objet à partir d'un tuple d'arguments
template<typename T, typename Tuple, std::size_t... I>
T construct_from_tuple(Tuple&& t, std::index_sequence<I...>) {
    return T(std::get<I>(std::forward<Tuple>(t))...);
}

template<typename T, typename Tuple>
T construct_from_tuple(Tuple&& t) {
    return construct_from_tuple<T>(
        std::forward<Tuple>(t),
        std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{}
    );
}

//Factory avec arguments différents par instance
template<typename T, typename ArgsTuples>
class FactoryWithDifferentArgs {
public:
    // Constructor qui prend un tuple de tuples d'arguments
    explicit FactoryWithDifferentArgs(ArgsTuples&& args_tuples)
        : args_tuples_(std::forward<ArgsTuples>(args_tuples)) 
    {
        init(std::make_index_sequence<std::tuple_size_v<ArgsTuples>>{});
    }

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
        for (std::size_t i = 0; i < instances.size(); ++i) {
            func(instances[i], i);
        }
    }

    // Nombre d'instances
    std::size_t count() const {
        return instances.size();
    }

private:
    ArgsTuples args_tuples_;
    std::vector<T> instances;

    // Initialisation à la compile
    template<std::size_t... I>
    void init(std::index_sequence<I...>) {
        (instances.push_back(construct_from_tuple<T>(
            std::get<I>(args_tuples_)
        )), ...);
    }
};

// Factory avec callback de construction personnalisée
template<typename T, std::size_t Num>
class FactoryWithBuilder {
public:
    // Constructor qui accepte un callback pour chaque instance
    template<typename BuilderFunc>
    explicit FactoryWithBuilder(BuilderFunc&& builder) {
        for (std::size_t i = 0; i < Num; ++i) {
            instances[i] = builder(i);
        }
    }

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

//Factory classique + méthode pour initialiser avec des args différents
template<typename T, std::size_t Num>
class FlexibleFactory {
public:
    // Constructeur par défaut
    FlexibleFactory() = default;

    // Initialisation avec les mêmes arguments
    template<typename... Args>
    void initialize_all(Args&&... args) {
        for (std::size_t i = 0; i < Num; ++i) {
            instances[i] = T(args...);
        }
    }

    // Initialisation avec un callback personnalisé par instance
    template<typename BuilderFunc>
    void initialize_with_builder(BuilderFunc&& builder) {
        for (std::size_t i = 0; i < Num; ++i) {
            instances[i] = builder(i);
        }
    }

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
