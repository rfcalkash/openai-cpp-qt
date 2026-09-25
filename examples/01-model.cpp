#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    // Calling API method is easy with some chosen free function 
    auto models = openai::model().list();
    std::cout << QJsonDocument(models["data"].toArray()[0].toObject()).toJson().toStdString() << std::endl;

    // Or if you prefer you can take back the instance reference and use member function
    auto& openai = openai::instance();
    auto models_other_approach = openai.model.list();
    std::cout << QJsonDocument(models_other_approach["data"].toArray()[1].toObject()).toJson().toStdString() << std::endl;

    // Helper member and free functions have the same signature
    auto model = openai::model().retrieve("text-davinci-003");
    std::cout << QJsonDocument(model).toJson().toStdString() << std::endl;
}
