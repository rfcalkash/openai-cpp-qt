#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    openai::instance().postAsync("moderations",
        QJsonObject{
            {"input", "I want to kill them."}
        },
        app.asyncDone([](openai::Json moderation) {
            std::cout << "Response is:\n"
                      << QJsonDocument(moderation).toJson(QJsonDocument::Indented).toStdString()
                      << '\n';
        }));

    return app.runUntilDone();
}
