#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    openai::instance().postAsync("fine-tunes",
        QJsonObject{
            {"training_file", "file-gD2KgQKsxcn6zguK0ETuodXO"}
        },
        app.asyncDone([&app](openai::Json upload) {
            std::cout << QJsonDocument(upload).toJson(QJsonDocument::Indented).toStdString() << '\n';

            openai::instance().getAsync("fine-tunes", app.asyncDone([&app](openai::Json files) {
                std::cout << QJsonDocument(files).toJson(QJsonDocument::Indented).toStdString() << '\n';

                openai::instance().getAsync("fine-tunes/ft-N9Zf32f3uzpoXxJLr14CnFy1", app.asyncDone([&app](openai::Json retrieve) {
                    std::cout << QJsonDocument(retrieve).toJson(QJsonDocument::Indented).toStdString() << '\n';

                    openai::instance().postAsync("fine-tunes/ft-N9Zf32f3uzpoXxJLr14CnFy1/cancel",
                        QJsonObject{},
                        app.asyncDone([&app](openai::Json cancel) {
                            std::cout << QJsonDocument(cancel).toJson(QJsonDocument::Indented).toStdString() << '\n';

                            openai::instance().getAsync("fine-tunes/ft-N9Zf32f3uzpoXxJLr14CnFy1/events", app.asyncDone([&app](openai::Json events) {
                                std::cout << QJsonDocument(events).toJson(QJsonDocument::Indented).toStdString() << '\n';

                                openai::instance().delAsync("models/any-model", app.asyncDone([](openai::Json res) {
                                    std::cout << QJsonDocument(res).toJson(QJsonDocument::Indented).toStdString() << '\n';
                                }));
                            }));
                        }));
                }));
            }));
        }));

    return app.runUntilDone();
}
