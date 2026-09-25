#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    openai::instance().postAsync("completions",
        QJsonObject{
            {"model", "text-davinci-003"},
            {"prompt", "Say this is a test"},
            {"max_tokens", 7},
            {"temperature", 0}
        },
        app.asyncDone([](openai::Json completion) {
            std::cout << "Response is:\n"
                      << QJsonDocument(completion).toJson(QJsonDocument::Indented).toStdString()
                      << '\n';
        }));

    return app.runUntilDone();
}
