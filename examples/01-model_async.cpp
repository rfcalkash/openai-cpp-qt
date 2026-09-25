#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    openai::instance().getAsync("models", app.asyncDone([&app](openai::Json models) {
        std::cout << QJsonDocument(models["data"].toArray()[0].toObject()).toJson().toStdString() << std::endl;

        openai::instance().getAsync("models", app.asyncDone([&app](openai::Json models_other) {
            std::cout << QJsonDocument(models_other["data"].toArray()[1].toObject()).toJson().toStdString() << std::endl;

            openai::instance().getAsync("models/text-davinci-003", app.asyncDone([](openai::Json model) {
                std::cout << QJsonDocument(model).toJson().toStdString() << std::endl;
            }));
        }));
    }));

    return app.runUntilDone();
}
