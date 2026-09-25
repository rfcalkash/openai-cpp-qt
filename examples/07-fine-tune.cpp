#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();
    
    // Upload
    {
        auto upload = openai::fineTune().create(QJsonObject{
            {"training_file", "file-gD2KgQKsxcn6zguK0ETuodXO"},
        });
        std::cout << QJsonDocument(upload).toJson(QJsonDocument::Indented).toStdString() << '\n';
    }

    // List fine tunes
    {
        auto files = openai::fineTune().list();
        std::cout << QJsonDocument(files).toJson(QJsonDocument::Indented).toStdString() << '\n'; 
    }

    // Retrieve
    {
        auto retrieve = openai::fineTune().retrieve("ft-N9Zf32f3uzpoXxJLr14CnFy1");
        std::cout << QJsonDocument(retrieve).toJson(QJsonDocument::Indented).toStdString() << '\n';
    }

    // Cancel
    {
        auto cancel = openai::fineTune().cancel("ft-N9Zf32f3uzpoXxJLr14CnFy1");
        std::cout << QJsonDocument(cancel).toJson(QJsonDocument::Indented).toStdString() << '\n';
    }

    // List Events
    {
        auto events = openai::fineTune().events("ft-N9Zf32f3uzpoXxJLr14CnFy1");
        std::cout << QJsonDocument(events).toJson(QJsonDocument::Indented).toStdString() << '\n';
    }

    // Delete
    {
        auto res = openai::fineTune().del("any-model");
        std::cout << QJsonDocument(res).toJson(QJsonDocument::Indented).toStdString() << '\n';
    }

}
