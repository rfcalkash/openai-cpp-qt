#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

void bar(openai::OpenAI& openai, OpenaiExampleApp& app, std::function<void()> then) {
    openai.postAsync("completions",
        QJsonObject{
            {"model", "text-davinci-003"},
            {"prompt", "Say bar() function called"}
        },
        app.asyncDone([then](openai::Json res) {
            std::cout << QJsonDocument(res).toJson().toStdString() << "\n\n";
            if (then) then();
        }));
}

int main() {
    OpenaiExampleApp app;

    auto openai_instance = openai::OpenAI::create();
    auto another_openai_instance = openai::OpenAI::create("wrong api key");
    // another_openai_instance.setThrowException(false);

    bar(openai_instance, app, [&app, &another_openai_instance]() {
        another_openai_instance.postAsync("completions",
            QJsonObject{
                {"model", "text-davinci-003"},
                {"prompt", "Say this should throw since token is invalid here"}
            },
            app.asyncDone([](openai::Json res) {
                std::cout << QJsonDocument(res).toJson().toStdString() << "\n\n";
            }));
    });

    return app.runUntilDone();
}
