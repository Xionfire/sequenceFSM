#include <iostream>
#include "facto_static.h"

// ============================================================================
// Classe d'exemple simple avec constructeur constexpr
// ============================================================================
class SimpleData {
public:
    int id;
    int value;

    // Constructeur constexpr - Peut être appelé à la compilation
    constexpr SimpleData(int id = 0, int value = 0)
        : id(id), value(value) {}

    // Constructeur par défaut
    constexpr SimpleData() : id(0), value(0) {}

    void display() const {
        std::cout << "  SimpleData[" << id << "] = " << value << "\n";
    }
};

// ============================================================================
// EXEMPLE 1 : Factory CompileTimeFactory avec instances créées à la compilation
// ============================================================================
int main() {
    std::cout << "===== EXEMPLE 1 : CompileTimeFactory =====\n";
    {
        // Les instances sont créées à la COMPILATION
        constexpr auto args = std::make_tuple(
            std::make_tuple(1, 100),
            std::make_tuple(2, 200),
            std::make_tuple(3, 300)
        );

        // Cette factory est constexpr - créée à la compilation
        constexpr CompileTimeFactory<SimpleData, decltype(args)> factory(args);

        // Les accès sont aussi constexpr
        constexpr auto first = factory[0];
        std::cout << "First instance (compile-time): [" << first.id << "] = " << first.value << "\n";

        std::cout << "Toutes les instances:\n";
        factory.for_each([](const SimpleData& obj, std::size_t i) {
            std::cout << "  Instance " << i << ": [" << obj.id << "] = " << obj.value << "\n";
        });

        std::cout << "Nombre d'instances: " << factory.count() << "\n";
    }

    std::cout << "\n===== EXEMPLE 2 : StaticFactory avec builder constexpr =====\n";
    {
        // Builder constexpr
        constexpr auto builder = [](std::size_t i) -> SimpleData {
            return SimpleData(static_cast<int>(i), static_cast<int>(i * 10));
        };

        // Factory avec builder à la compilation
        constexpr StaticFactory<SimpleData, 4> factory = 
            StaticFactory<SimpleData, 4>::create_with_builder(builder);

        std::cout << "Instances créées avec builder constexpr:\n";
        factory.for_each([](const SimpleData& obj, std::size_t i) {
            std::cout << "  Instance " << i << ": [" << obj.id << "] = " << obj.value << "\n";
        });
    }

    std::cout << "\n===== EXEMPLE 3 : GlobalStaticFactory (Mémoire statique globale) =====\n";
    {
        // Définition de la factory globale statique
        constexpr auto global_args = std::make_tuple(
            std::make_tuple(10, 1000),
            std::make_tuple(20, 2000),
            std::make_tuple(30, 3000)
        );

        // Cette factory est stockée dans la mémoire statique du programme
        // (section .rodata pour les données const)
        class GlobalFactory {
        public:
            static const CompileTimeFactory<SimpleData, decltype(global_args)>& get() {
                // Instance unique et statique
                static constexpr auto args = std::make_tuple(
                    std::make_tuple(10, 1000),
                    std::make_tuple(20, 2000),
                    std::make_tuple(30, 3000)
                );
                static constexpr CompileTimeFactory<SimpleData, decltype(args)> factory{args};
                return factory;
            }
        };

        const auto& factory = GlobalFactory::get();
        std::cout << "Instances de la factory globale statique:\n";
        factory.for_each([](const SimpleData& obj, std::size_t i) {
            std::cout << "  Instance " << i << ": [" << obj.id << "] = " << obj.value << "\n";
        });

        // Ces instances sont toujours les mêmes sans ré-initialisation
        std::cout << "Accès à nouveau (mêmes instances en mémoire statique):\n";
        const auto& factory2 = GlobalFactory::get();
        std::cout << "  factory[0] = " << factory2[0].value << "\n";
        std::cout << "  factory[1] = " << factory2[1].value << "\n";
    }

    std::cout << "\n===== EXEMPLE 4 : MixedStaticFactory (Compile-time construction) =====\n";
    {
        // Factory avec construction constexpr
        constexpr MixedStaticFactory<SimpleData, 3> factory(42);

        std::cout << "Instances créées avec même argument (42):\n";
        factory.for_each([](const SimpleData& obj, std::size_t i) {
            std::cout << "  Instance " << i << ": value = " << obj.value << "\n";
        });
    }

    std::cout << "\n===== RÉSUMÉ : Avantages du stockage statique =====\n";
    std::cout << "✅ Les instances sont créées à la COMPILATION (compile-time)\n";
    std::cout << "✅ Zéro coût runtime - pas d'initialisation dynamique\n";
    std::cout << "✅ Mémoire optimisée - stockage en .rodata (read-only)\n";
    std::cout << "✅ Type-safe et constexpr-compatible\n";
    std::cout << "✅ Peut être utilisé dans des contextes constexpr\n";

    return 0;
}
