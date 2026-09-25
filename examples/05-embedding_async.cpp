#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    openai::instance().postAsync("embeddings",
        QJsonObject{
            {"model", "text-embedding-ada-002"},
            {"input", "The food was delicious and the waiter..."}
        },
        app.asyncDone([](openai::Json res) {
            std::cout << "Response is:\n"
                      << QJsonDocument(res).toJson(QJsonDocument::Indented).toStdString()
                      << '\n';
        }));

    return app.runUntilDone();
}
