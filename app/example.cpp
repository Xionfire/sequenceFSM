#include <iostream>
#include "facto_v2.h"

// Classe d'exemple
class MyClass {
public:
    int id;
    std::string name;
    double value;

    // Constructeurs multiples
    MyClass() : id(0), name("default"), value(0.0) {
        std::cout << "MyClass() default constructor\n";
    }

    MyClass(int id) : id(id), name("obj_" + std::to_string(id)), value(0.0) {
        std::cout << "MyClass(int) constructor: id=" << id << "\n";
    }

    MyClass(int id, const std::string& name) 
        : id(id), name(name), value(0.0) {
        std::cout << "MyClass(int, string) constructor: id=" << id 
                  << ", name=" << name << "\n";
    }

    MyClass(int id, const std::string& name, double value)
        : id(id), name(name), value(value) {
        std::cout << "MyClass(int, string, double) constructor: id=" << id 
                  << ", name=" << name << ", value=" << value << "\n";
    }

    void display() const {
        std::cout << "  [" << id << "] " << name << " = " << value << "\n";
    }
};

int main() {
    std::cout << "===== VERSION 1 : Avec tuples d'arguments =====\n";
    {
        // Chaque instance a ses propres arguments
        auto args = std::make_tuple(
            std::make_tuple(1, std::string("Alice"), 10.5),
            std::make_tuple(2, std::string("Bob"), 20.3),
            std::make_tuple(3, std::string("Charlie"), 30.1)
        );
        
        FactoryWithDifferentArgs<MyClass, decltype(args)> factory(std::move(args));
        
        std::cout << "Instances créées:\n";
        factory.for_each([](const MyClass& obj, std::size_t i) {
            std::cout << "Instance " << i << ": ";
            obj.display();
        });
    }

    std::cout << "\n===== VERSION 2 : Avec builder callback =====\n";
    {
        // Utiliser un lambda qui construit différemment selon l'index
        FactoryWithBuilder<MyClass, 4> factory([](std::size_t i) -> MyClass {
            switch(i) {
                case 0:
                    return MyClass(10);
                case 1:
                    return MyClass(20, "Instance_1");
                case 2:
                    return MyClass(30, "Instance_2", 99.9);
                default:
                    return MyClass();
            }
        });

        std::cout << "Instances créées:\n";
        factory.for_each([](const MyClass& obj, std::size_t i) {
            std::cout << "Instance " << i << ": ";
            obj.display();
        });
    }

    std::cout << "\n===== VERSION 3 : Factory flexible =====\n";
    {
        FlexibleFactory<MyClass, 3> factory;

        // Initialiser avec un builder
        factory.initialize_with_builder([](std::size_t i) -> MyClass {
            return MyClass(100 + i, "Flexible_" + std::to_string(i), i * 1.5);
        });

        std::cout << "Instances créées:\n";
        factory.for_each([](const MyClass& obj, std::size_t i) {
            std::cout << "Instance " << i << ": ";
            obj.display();
        });

        std::cout << "\nAu besoin, on peut aussi initialiser les mêmes:\n";
        factory.initialize_all(999, "Same");
        factory.for_each([](const MyClass& obj, std::size_t i) {
            std::cout << "Instance " << i << ": ";
            obj.display();
        });
    }

    return 0;
}
