#include <array>
#include <cstddef>
#include <utility>
#include <tuple>

// ============================================================================
// FACTORY AVEC STOCKAGE STATIQUE (COMPILE-TIME)
// ============================================================================

// Utilitaire pour construire un objet à partir d'un tuple d'arguments (constexpr)
template<typename T, typename Tuple, std::size_t... I>
constexpr T construct_from_tuple_impl(const Tuple& t, std::index_sequence<I...>) {
    return T(std::get<I>(t)...);
}

template<typename T, typename Tuple>
constexpr T construct_from_tuple(const Tuple& t) {
    return construct_from_tuple_impl<T>(
        t,
        std::make_index_sequence<std::tuple_size_v<Tuple>>{}
    );
}

// ============================================================================
// Factory CONSTEXPR avec arguments différents - Stockage compile-time
// Les instances sont crées à la COMPILATION et stockées dans une std::array
// ============================================================================
template<typename T, typename ArgsTuples>
class CompileTimeFactory {
private:
    static constexpr std::size_t Num = std::tuple_size_v<ArgsTuples>;
    
    // Helper pour créer le tableau à la compilation
    template<std::size_t... I>
    static constexpr auto make_storage(const ArgsTuples& args, std::index_sequence<I...>) {
        return std::array<T, Num>{{
            construct_from_tuple<T>(std::get<I>(args))...
        }};
    }

public:
    // Constructor constexpr - s'évalue à la compilation
    constexpr explicit CompileTimeFactory(const ArgsTuples& args_tuples)
        : instances_(make_storage(args_tuples, std::make_index_sequence<Num>{}))
    {}

    // Accès par ID - constexpr compatible
    constexpr T& get_instance(std::size_t id) {
        return instances_[id];
    }

    constexpr const T& get_instance(std::size_t id) const {
        return instances_[id];
    }

    // Accès avec [] - constexpr compatible
    constexpr T& operator[](std::size_t id) {
        return instances_[id];
    }

    constexpr const T& operator[](std::size_t id) const {
        return instances_[id];
    }

    // Parcours de toutes les instances
    template<typename Func>
    constexpr void for_each(Func&& func) {
        for (std::size_t i = 0; i < Num; ++i) {
            func(instances_[i], i);
        }
    }

    // Nombre d'instances - constexpr
    static constexpr std::size_t count() {
        return Num;
    }

private:
    std::array<T, Num> instances_;
};

// ============================================================================
// Factory Flexible avec deux modes - Statique et Runtime
// ============================================================================
template<typename T, std::size_t Num>
class StaticFactory {
public:
    // Constructeur par défaut
    constexpr StaticFactory() = default;

    // Mode 1 : Initialisation compile-time avec mêmes arguments (via constexpr)
    template<typename... Args>
    constexpr static StaticFactory create_all(Args&&... args) {
        StaticFactory factory;
        return factory;
    }

    // Mode 2 : Initialisation avec un builder (si constexpr compatible)
    template<typename BuilderFunc>
    constexpr static StaticFactory create_with_builder(const BuilderFunc& builder) {
        return StaticFactory(builder, std::make_index_sequence<Num>{});
    }

    // Accès par ID
    constexpr T& get_instance(std::size_t id) {
        return instances_[id];
    }

    constexpr const T& get_instance(std::size_t id) const {
        return instances_[id];
    }

    // Accès avec []
    constexpr T& operator[](std::size_t id) {
        return instances_[id];
    }

    constexpr const T& operator[](std::size_t id) const {
        return instances_[id];
    }

    // Parcours de toutes les instances
    template<typename Func>
    constexpr void for_each(Func&& func) {
        for (std::size_t i = 0; i < Num; ++i) {
            func(instances_[i], i);
        }
    }

    // Nombre d'instances - compile-time
    static constexpr std::size_t count() {
        return Num;
    }

private:
    std::array<T, Num> instances_{};

    // Constructor privé pour create_with_builder
    template<typename BuilderFunc, std::size_t... I>
    constexpr StaticFactory(const BuilderFunc& builder, std::index_sequence<I...>)
        : instances_{{builder(I)...}}
    {}
};

// ============================================================================
// Factory SINGLETON STATIQUE - Une seule instance à la compilation
// ============================================================================
template<typename T, typename ArgsTuple>
class StaticSingletonFactory {
public:
    // Accès à l'instance unique - statique et compile-time
    static constexpr T& instance() {
        return storage_.instance_;
    }

    static constexpr const T& instance() const {
        return storage_.instance_;
    }

    // Pas de constructeur public
    StaticSingletonFactory() = delete;

private:
    // Créer l'instance une seule fois à la compilation
    struct Storage {
        static constexpr ArgsTuple default_args = ArgsTuple{};
        constexpr Storage()
            : instance_(construct_from_tuple<T>(default_args))
        {}
        T instance_;
    };

    static constexpr Storage storage_{};
};

// ============================================================================
// Factory GLOBAL STATIQUE - Stockage dans la section .data/.rodata
// ============================================================================
template<typename T, typename ArgsTuples>
class GlobalStaticFactory {
public:
    static const CompileTimeFactory<T, ArgsTuples>& get() {
        return factory_;
    }

    // Accès direct à une instance
    static const T& get_instance(std::size_t id) {
        return factory_.get_instance(id);
    }

    static constexpr std::size_t count() {
        return std::tuple_size_v<ArgsTuples>;
    }

private:
    // Instance globale statique - stockée en mémoire statique
    static constexpr ArgsTuples default_args = ArgsTuples{};
    static constexpr CompileTimeFactory<T, ArgsTuples> factory_{default_args};
};

// ============================================================================
// MACRO pour créer facilement une factory statique globale
// ============================================================================
#define STATIC_FACTORY(FactoryClass, TypeName, ArgsTupleVar) \
    struct FactoryClass { \
        static const auto& get() { \
            static constexpr CompileTimeFactory<TypeName, decltype(ArgsTupleVar)> \
                instance{ArgsTupleVar}; \
            return instance; \
        } \
    }

// Alternative avec initialisation runtime à partir de compile-time
template<typename T, std::size_t Num>
class MixedStaticFactory {
public:
    // Création constexpr à la compilation
    template<typename... Args>
    constexpr MixedStaticFactory(Args&&... args) {
        init_all(std::forward<Args>(args)...);
    }

    // Accès par ID
    constexpr T& get_instance(std::size_t id) {
        return instances_[id];
    }

    constexpr const T& get_instance(std::size_t id) const {
        return instances_[id];
    }

    // Accès avec []
    constexpr T& operator[](std::size_t id) {
        return instances_[id];
    }

    constexpr const T& operator[](std::size_t id) const {
        return instances_[id];
    }

    // Nombre d'instances
    static constexpr std::size_t count() {
        return Num;
    }

private:
    std::array<T, Num> instances_{};

    template<typename... Args>
    constexpr void init_all(Args&&... args) {
        for (std::size_t i = 0; i < Num; ++i) {
            instances_[i] = T(args...);
        }
    }
};
