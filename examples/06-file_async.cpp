#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();

    // File upload uses multipart -> use the synchronous helper to obtain the id first.
    auto upload = openai::instance().file.upload(QJsonObject{
        {"file", "finetune1.jsonl"},
        {"purpose", "fine-tune"}
    });
    std::cout << QJsonDocument(upload).toJson(QJsonDocument::Indented).toStdString() << "\n\n";
    QString uploaded_file_id = upload["id"].toString();

    openai::instance().getAsync("files", app.asyncDone([&app, uploaded_file_id](openai::Json files) {
        std::cout << QJsonDocument(files).toJson(QJsonDocument::Indented).toStdString() << "\n\n";

        openai::instance().getAsync("files/" + uploaded_file_id, app.asyncDone([&app, uploaded_file_id](openai::Json retrieve) {
            std::cout << QJsonDocument(retrieve).toJson(QJsonDocument::Indented).toStdString() << "\n\n";

            openai::instance().getAsync("files/" + uploaded_file_id + "/content", app.asyncDone([&app, uploaded_file_id](openai::Json content) {
                std::cout << QJsonDocument(content).toJson(QJsonDocument::Indented).toStdString() << "\n\n";

                openai::instance().delAsync("files/" + uploaded_file_id, app.asyncDone([](openai::Json deletion) {
                    std::cout << QJsonDocument(deletion).toJson(QJsonDocument::Indented).toStdString() << "\n\n";
                }));
            }));
        }));
    }));

    return app.runUntilDone();
}
