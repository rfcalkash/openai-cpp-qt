#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    {
        auto moderation = openai::moderation().create(QJsonObject{
            {"input", "I want to kill them."}
        });
        std::cout << "Response is:\n" << QJsonDocument(moderation).toJson(QJsonDocument::Indented).toStdString() << '\n';
    }
}

