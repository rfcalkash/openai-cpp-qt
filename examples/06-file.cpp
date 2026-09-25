#include "openai.hpp"
#include "OpenaiExampleApp.hpp"

#include <iostream>

int main() {
    OpenaiExampleApp app;
    openai::start();
    
    // Upload file
    auto upload = openai::file().upload(QJsonObject{
        {"file", "finetune1.jsonl"},
        {"purpose", "fine-tune"}
    });
    std::cout << QJsonDocument(upload).toJson(QJsonDocument::Indented).toStdString() << "\n\n";
    QString uploaded_file_id = upload["id"].toString(); // retrieve file id of uploaded file file-....


    // List files
    auto files = openai::file().list();
    std::cout << QJsonDocument(files).toJson(QJsonDocument::Indented).toStdString() << "\n\n"; 

    // Retrieve file
    auto retrieve = openai::file().retrieve(uploaded_file_id); // file-....
    std::cout << QJsonDocument(retrieve).toJson(QJsonDocument::Indented).toStdString() << "\n\n"; 


    // Retrieve file content
    try {
        auto content = openai::file().content(uploaded_file_id); // file-....
        std::cout << QJsonDocument(content).toJson(QJsonDocument::Indented).toStdString() << "\n\n"; 
    } catch(std::exception const& e) {
        std::cerr << "You might have this exception because you have a free account " << e.what() << "\n\n";
    }

    // Delete file
    auto deletion = openai::file().del(uploaded_file_id);
    std::cout << QJsonDocument(deletion).toJson(QJsonDocument::Indented).toStdString() << "\n\n"; 
}
