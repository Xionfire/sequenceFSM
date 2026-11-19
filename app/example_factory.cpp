#include <iostream>
#include "../include/Factory.h"

// ============================================================================
// EXEMPLE 1 : Classe simple sans communication
// ============================================================================
class SimpleDevice {
private:
    int device_id;
    int temperature;

public:
    SimpleDevice(int id = 0) : device_id(id), temperature(0) {}

    void set_temperature(int temp) {
        temperature = temp;
    }

    int get_temperature() const {
        return temperature;
    }

    void display() const {
        std::cout << "Device " << device_id << " : " << temperature << " °C\n";
    }
};

// ============================================================================
// EXEMPLE 2 : Classe avec communication inter-instances
// ============================================================================
class ControllerWithMessaging : public ICommunicable<int> {
private:
    int controller_id;
    bool alert_flag;
    Message<int> last_message;

public:
    ControllerWithMessaging(int id = 0) 
        : controller_id(id), alert_flag(false) {}

    void send_alert(int destination_id) {
        // Simulé localement, sera géré par la factory
        std::cout << "Controller " << controller_id 
                  << " sends alert to Controller " << destination_id << "\n";
    }

    void send_message(const Message<int>& msg) override {
        // Appelé par la factory pour envoyer un message
        std::cout << "Controller " << controller_id 
                  << " sending to " << msg.to_instance_id 
                  << " with data: " << msg.data << "\n";
    }

    void receive_message(Message<int>& msg) override {
        if (msg.to_instance_id == controller_id && msg.is_valid) {
            std::cout << "Controller " << controller_id 
                      << " RECEIVED from " << msg.from_instance_id 
                      << " : FLAG = " << msg.data << "\n";
            last_message = msg;
            if (msg.data > 0) {
                alert_flag = true;
            }
        }
    }

    bool has_alert() const {
        return alert_flag;
    }

    void reset_alert() {
        alert_flag = false;
    }

    void display() const {
        std::cout << "Controller " << controller_id 
                  << " - Alert: " << (alert_flag ? "YES" : "NO") << "\n";
    }
};

// ============================================================================
// EXEMPLE 3 : Classe avec état et comportement complexe
// ============================================================================
class Pump {
private:
    int pump_id;
    bool is_running;
    int flow_rate;
    int error_code;

public:
    Pump(int id = 0, int initial_flow = 100)
        : pump_id(id), is_running(false), flow_rate(initial_flow), error_code(0) {}

    void start() {
        is_running = true;
        error_code = 0;
        std::cout << "Pump " << pump_id << " STARTED (Flow: " 
                  << flow_rate << " L/min)\n";
    }

    void stop() {
        is_running = false;
        std::cout << "Pump " << pump_id << " STOPPED\n";
    }

    void set_flow_rate(int rate) {
        if (rate >= 0 && rate <= 200) {
            flow_rate = rate;
        } else {
            error_code = 1; // Erreur : débit hors limites
        }
    }

    bool is_operational() const {
        return is_running && error_code == 0;
    }

    void display() const {
        std::cout << "Pump " << pump_id << " : "
                  << (is_running ? "RUNNING" : "STOPPED") 
                  << " - Flow: " << flow_rate 
                  << " L/min - Error: " << error_code << "\n";
    }
};

// ============================================================================
// TESTS ET DÉMONSTRATION
// ============================================================================
int main() {
    std::cout << "\n========== EXEMPLE 1 : Factory avec SimpleDevice ==========\n";
    {
        // Crée une factory de 3 SimpleDevice avec id=0
        FactorySimple<SimpleDevice, 3> device_factory(0);

        // Initialise chaque appareil avec une température différente
        device_factory[0].set_temperature(20);
        device_factory[1].set_temperature(45);
        device_factory[2].set_temperature(72);

        // Affiche tous les appareils
        std::cout << "\nTous les appareils :\n";
        device_factory.for_each([](SimpleDevice& dev, size_t idx) {
            std::cout << "  [" << idx << "] ";
            dev.display();
        });
    }

    std::cout << "\n========== EXEMPLE 2 : Factory avec Communication ==========\n";
    {
        // Crée une factory de 4 contrôleurs avec communication
        FactorySimple<ControllerWithMessaging, 4> controller_factory(0);

        // Les ID sont définis lors de la création
        for (size_t i = 0; i < controller_factory.count(); ++i) {
            controller_factory[i].display();
        }

        std::cout << "\nSimulation de messages inter-instances :\n";

        // Controller 0 envoie un flag (1 = alerte) à Controller 2
        controller_factory.send_message(0, 2, 1);

        // Controller 1 envoie un flag (0 = OK) à Controller 3
        controller_factory.send_message(1, 3, 0);

        std::cout << "\nAprès réception des messages :\n";
        controller_factory.for_each([](ControllerWithMessaging& ctrl, size_t) {
            ctrl.display();
        });
    }

    std::cout << "\n========== EXEMPLE 3 : Factory avec Pump ==========\n";
    {
        // Crée une factory de 5 pompes, chacune avec un débit initial différent
        FactorySimple<Pump, 5> pump_factory(0, 100);

        std::cout << "\nInitialisation des pompes :\n";
        for (size_t i = 0; i < pump_factory.count(); ++i) {
            pump_factory[i].set_flow_rate(50 + i * 30);
        }

        std::cout << "\nDémarrage des pompes 0, 2, 4 :\n";
        pump_factory[0].start();
        pump_factory[2].start();
        pump_factory[4].start();

        std::cout << "\nÉtat de toutes les pompes :\n";
        pump_factory.for_each([](Pump& pump, size_t) {
            pump.display();
        });

        std::cout << "\nArrêt de la pompe 2 :\n";
        pump_factory[2].stop();

        std::cout << "\nÉtat final :\n";
        pump_factory.for_each([](Pump& pump, size_t) {
            pump.display();
        });
    }

    std::cout << "\n========== INFORMATIONS COMPILE-TIME ==========\n";
    std::cout << "SimpleDevice Factory:\n"
              << "  - Nombre d'instances: " 
              << FactorySimple<SimpleDevice, 3>::count() << "\n"
              << "  - Taille mémoire totale: " 
              << FactorySimple<SimpleDevice, 3>::total_size() << " bytes\n";

    std::cout << "Pump Factory:\n"
              << "  - Nombre d'instances: " 
              << FactorySimple<Pump, 5>::count() << "\n"
              << "  - Taille mémoire totale: " 
              << FactorySimple<Pump, 5>::total_size() << " bytes\n";

    return 0;
}
