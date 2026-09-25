// The MIT License (MIT)
//
// Copyright (c) 2023 Olrea, Florian Dang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef OPENAI_HPP_
#define OPENAI_HPP_

#if OPENAI_VERBOSE_OUTPUT
#pragma message ("OPENAI_VERBOSE_OUTPUT is ON")
#endif

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <mutex>
#include <cstdlib>
#include <map>
#include <functional>
#include <memory>

#include <QString>
#include <QMap>
#include <QPair>
#include <QFileInfo>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>
#include <QUrlQuery>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFile>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QNetworkProxy>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>


namespace openai {

namespace _detail {

using Json = QJsonObject;

inline QString jsonToString(const QJsonObject& obj, bool compact = true) {
    const auto fmt = compact ? QJsonDocument::Compact : QJsonDocument::Indented;
    return QString::fromUtf8(QJsonDocument(obj).toJson(fmt));
}

inline QString jsonToString(const QJsonArray& arr, bool compact = true) {
    const auto fmt = compact ? QJsonDocument::Compact : QJsonDocument::Indented;
    return QString::fromUtf8(QJsonDocument(arr).toJson(fmt));
}

inline QJsonObject stringToJsonObject(const QString& str) {
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
    return doc.object();
}

inline QJsonArray stringToJsonArray(const QString& str) {
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
    return doc.array();
}

struct Response {
    QString text;
    bool    is_error;
    QString error_message;
};

// QNetworkAccessManager-based Session
class Session {
public:
    explicit Session(bool throw_exception, QNetworkAccessManager* network_manager = nullptr)
        : external_manager_{network_manager},
          throw_exception_{throw_exception}
    {
        if (external_manager_ == nullptr) {
            owned_manager_ = std::make_unique<QNetworkAccessManager>();
        }
        ignoreSSL();
    }

    Session(bool throw_exception, const QString& proxy_url)
        : Session(throw_exception, nullptr)
    {
        setProxyUrl(proxy_url);
    }

    Session(Session&&) = default;
    Session& operator=(Session&&) = default;
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    ~Session() {
        if (multi_part_ != nullptr) {
            delete multi_part_;
        }
    }

    QNetworkAccessManager* manager() {
        return external_manager_ != nullptr ? external_manager_ : owned_manager_.get();
    }

    void ignoreSSL() {
        QSslConfiguration ssl_config = QSslConfiguration::defaultConfiguration();
        ssl_config.setPeerVerifyMode(QSslSocket::VerifyNone);
        request_.setSslConfiguration(ssl_config);
    }

    void setUrl(const QString& url) {
        url_ = url;
        request_.setUrl(QUrl(url_));
    }

    void setToken(const QString& token, const QString& organization) {
        token_ = token;
        organization_ = organization;
    }

    void setProxyUrl(const QString& url) {
        proxy_url_ = url;
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        QUrl proxy_url(url);
        proxy.setHostName(proxy_url.host());
        proxy.setPort(static_cast<quint16>(proxy_url.port(80)));
        if (!proxy_url.userName().isEmpty()) {
            proxy.setUser(proxy_url.userName());
        }
        if (!proxy_url.password().isEmpty()) {
            proxy.setPassword(proxy_url.password());
        }
        manager()->setProxy(proxy);
    }

    void setBeta(const QString& beta) { beta_ = beta; }

    void setBody(const QString& data) {
        body_ = data.toUtf8();
    }

    void setMultiformPart(const QPair<QString, QString>& filefield_and_filepath, const QMap<QString, QString>& fields);

    Response getPrepare();
    Response postPrepare(const QString& contentType = QString());
    Response deletePrepare();

    using ResponseCallback = std::function<void(Response)>;

    void getAsync(ResponseCallback cb);
    void postAsync(const QString& contentType, ResponseCallback cb);
    void deleteAsync(ResponseCallback cb);

    QString easyEscape(const QString& text);

private:
    void applyHeaders(const QString& contentType);
    void makeRequestAsync(QNetworkAccessManager::Operation op, const QString& contentType, ResponseCallback cb);
    Response makeRequestSync(QNetworkAccessManager::Operation op, const QString& contentType);

private:
    QNetworkRequest request_;
    QNetworkAccessManager* external_manager_ = nullptr;
    std::unique_ptr<QNetworkAccessManager> owned_manager_;
    QHttpMultiPart* multi_part_ = nullptr;

    QByteArray body_;
    QString url_;
    QString proxy_url_;
    QString token_;
    QString organization_;
    QString beta_;

    bool throw_exception_;
    std::unique_ptr<std::mutex> mutex_request_ = std::make_unique<std::mutex>();
};

inline void Session::setMultiformPart(const QPair<QString, QString>& filefield_and_filepath, const QMap<QString, QString>& fields) {
    if (multi_part_ != nullptr) {
        delete multi_part_;
        multi_part_ = nullptr;
    }

    multi_part_ = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart file_part;
    const QString header = QStringLiteral("form-data; name=\"%1\"; filename=\"%2\"")
                               .arg(filefield_and_filepath.first,
                                    QFileInfo(filefield_and_filepath.second).fileName());
    file_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(header));

    QFile* file = new QFile(filefield_and_filepath.second);
    if (!file->open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Cannot open file: " + filefield_and_filepath.second.toStdString());
    }
    file->setParent(multi_part_);
    file_part.setBodyDevice(file);
    multi_part_->append(file_part);

    for (auto it = fields.cbegin(); it != fields.cend(); ++it) {
        QHttpPart text_part;
        const QString text_header = QStringLiteral("form-data; name=\"%1\"").arg(it.key());
        text_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(text_header));
        text_part.setBody(it.value().toUtf8());
        multi_part_->append(text_part);
    }
}

inline void Session::applyHeaders(const QString& contentType) {
    if (!contentType.isEmpty()) {
        request_.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    }
    request_.setRawHeader("Authorization", QStringLiteral("Bearer %1").arg(token_).toUtf8());
    if (!organization_.isEmpty()) {
        request_.setRawHeader("OpenAI-Organization", organization_.toUtf8());
    }
    if (!beta_.isEmpty()) {
        request_.setRawHeader("OpenAI-Beta", beta_.toUtf8());
    }
}

inline void Session::makeRequestAsync(QNetworkAccessManager::Operation op, const QString& contentType, ResponseCallback cb) {
    std::lock_guard<std::mutex> lock(*mutex_request_);

    applyHeaders(contentType);

    QNetworkReply* reply = nullptr;
    switch (op) {
        case QNetworkAccessManager::GetOperation:
            reply = manager()->get(request_);
            break;
        case QNetworkAccessManager::PostOperation:
            if (multi_part_ != nullptr) {
                reply = manager()->post(request_, multi_part_);
                multi_part_->setParent(reply);
                multi_part_ = nullptr;
            } else {
                reply = manager()->post(request_, body_);
            }
            break;
        case QNetworkAccessManager::DeleteOperation:
            reply = manager()->deleteResource(request_);
            break;
        default:
            throw std::runtime_error("Unsupported HTTP operation");
    }

    QObject::connect(reply, &QNetworkReply::finished, [this, reply, cb]() {
        Response response;
        if (reply->error() != QNetworkReply::NoError) {
            response.is_error = true;
            response.error_message = QStringLiteral("OpenAI request failed: %1").arg(reply->errorString());
            response.text = QString::fromUtf8(reply->readAll());
            if (throw_exception_) {
                reply->deleteLater();
                throw std::runtime_error(response.error_message.toStdString());
            }
            std::cerr << response.error_message.toStdString() << '\n';
        } else {
            response.is_error = false;
            response.text = QString::fromUtf8(reply->readAll());
        }
        reply->deleteLater();
        cb(response);
    });
}

inline Response Session::makeRequestSync(QNetworkAccessManager::Operation op, const QString& contentType) {
    Response response;
    QEventLoop loop;

    makeRequestAsync(op, contentType, [&](Response r) {
        response = std::move(r);
        loop.quit();
    });

    loop.exec();
    return response;
}

inline Response Session::getPrepare() {
    return makeRequestSync(QNetworkAccessManager::GetOperation, QString());
}

inline Response Session::postPrepare(const QString& contentType) {
    return makeRequestSync(QNetworkAccessManager::PostOperation, contentType);
}

inline Response Session::deletePrepare() {
    return makeRequestSync(QNetworkAccessManager::DeleteOperation, QString());
}

inline void Session::getAsync(ResponseCallback cb) {
    makeRequestAsync(QNetworkAccessManager::GetOperation, QString(), std::move(cb));
}

inline void Session::postAsync(const QString& contentType, ResponseCallback cb) {
    makeRequestAsync(QNetworkAccessManager::PostOperation, contentType, std::move(cb));
}

inline void Session::deleteAsync(ResponseCallback cb) {
    makeRequestAsync(QNetworkAccessManager::DeleteOperation, QString(), std::move(cb));
}

inline QString Session::easyEscape(const QString& text) {
    return QUrl::toPercentEncoding(text);
}

class OpenAI;

struct CategoryModel {
    Json list();
    Json retrieve(const QString& model);
    CategoryModel(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct CategoryAssistants {
    Json create(Json input);
    Json retrieve(const QString& assistants);
    Json modify(const QString& assistants, Json input);
    Json del(const QString& assistants);
    Json list();
    Json createFile(const QString& assistants, Json input);
    Json retrieveFile(const QString& assistants, const QString& files);
    Json delFile(const QString& assistants, const QString& files);
    Json listFile(const QString& assistants);
    CategoryAssistants(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct CategoryThreads {
    Json create();
    Json retrieve(const QString& threads);
    Json modify(const QString& threads, Json input);
    Json del(const QString& threads);
    Json list();

    Json createMessage(const QString& threads, Json input);
    Json retrieveMessage(const QString& threads, const QString& messages);
    Json modifyMessage(const QString& threads, const QString& messages, Json input);
    Json listMessage(const QString& threads);
    Json retrieveMessageFile(const QString& threads, const QString& messages, const QString& files);
    Json listMessageFile(const QString& threads, const QString& messages);

    Json createRun(const QString& threads, Json input);
    Json retrieveRun(const QString& threads, const QString& runs);
    Json modifyRun(const QString& threads, const QString& runs, Json input);
    Json listRun(const QString& threads);
    Json submitToolOutputsToRun(const QString& threads, const QString& runs, Json input);
    Json cancelRun(const QString& threads, const QString& runs);
    Json createThreadAndRun(Json input);
    Json retrieveRunStep(const QString& threads, const QString& runs, const QString& steps);
    Json listRunStep(const QString& threads, const QString& runs);

    CategoryThreads(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct CategoryCompletion {
    Json create(Json input);
    CategoryCompletion(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct CategoryChat {
    Json create(Json input);
    CategoryChat(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct CategoryAudio {
    Json transcribe(Json input);
    Json translate(Json input);
    CategoryAudio(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct CategoryEdit {
    Json create(Json input);
    CategoryEdit(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct CategoryImage {
    Json create(Json input);
    Json edit(Json input);
    Json variation(Json input);
    CategoryImage(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct CategoryEmbedding {
    Json create(Json input);
    CategoryEmbedding(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct FileRequest {
    QString file;
    QString purpose;
};

struct CategoryFile {
    Json list();
    Json upload(Json input);
    Json del(const QString& file);
    Json retrieve(const QString& file_id);
    Json content(const QString& file_id);
    CategoryFile(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct CategoryFineTune {
    Json create(Json input);
    Json list();
    Json retrieve(const QString& fine_tune_id);
    Json content(const QString& fine_tune_id);
    Json cancel(const QString& fine_tune_id);
    Json events(const QString& fine_tune_id);
    Json del(const QString& model);
    CategoryFineTune(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

struct CategoryModeration {
    Json create(Json input);
    CategoryModeration(OpenAI& openai) : openai_{openai} {}
private:
    OpenAI& openai_;
};

class OpenAI {
public:
    static OpenAI create(const QString& token = QString(),
                         const QString& organization = QString(),
                         bool throw_exception = true,
                         const QString& api_base_url = QString(),
                         const QString& beta = QString()) {
        return OpenAI(token, organization, throw_exception, api_base_url, beta, nullptr);
    }

    static OpenAI createWithNetworkManager(QNetworkAccessManager* network_manager,
                                           const QString& token = QString(),
                                           const QString& organization = QString(),
                                           bool throw_exception = true,
                                           const QString& api_base_url = QString(),
                                           const QString& beta = QString()) {
        return OpenAI(token, organization, throw_exception, api_base_url, beta, network_manager);
    }

    OpenAI(const OpenAI&)               = delete;
    OpenAI& operator=(const OpenAI&)    = delete;
    OpenAI(OpenAI&&)                    = default;
    OpenAI& operator=(OpenAI&&)         = delete;

    void setToken(const QString& token, const QString& organization = QString()) { session_.setToken(token, organization); }
    void setProxy(const QString& url) { session_.setProxyUrl(url); }
    void setBeta(const QString& beta) { session_.setBeta(beta); }
    void setMultiformPart(const QPair<QString, QString>& filefield_and_filepath, const QMap<QString, QString>& fields) { session_.setMultiformPart(filefield_and_filepath, fields); }

    Json post(const QString& suffix, const QString& data, const QString& contentType) {
        setParameters(suffix, data, contentType);
        auto response = session_.postPrepare(contentType);
        if (response.is_error){
            trigger_error(response.error_message);
        }
        return parseResponse(response);
    }

    Json get(const QString& suffix, const QString& data = QString()) {
        setParameters(suffix, data);
        auto response = session_.getPrepare();
        if (response.is_error) { trigger_error(response.error_message); }

        Json json;
        if (isJson(response.text)) {
            json = stringToJsonObject(response.text);
            checkResponse(json);
        }
        else {
          #if OPENAI_VERBOSE_OUTPUT
            std::cerr << "Response is not a valid JSON\n";
            std::cout << "<< " << response.text.toStdString() << "\n";
          #endif
            json = QJsonObject{{"Result", response.text}};
        }
        return json;
    }

    Json post(const QString& suffix, const Json& json, const QString& contentType="application/json") {
        return post(suffix, jsonToString(json), contentType);
    }

    Json del(const QString& suffix) {
        setParameters(suffix, QString());
        auto response = session_.deletePrepare();
        if (response.is_error) { trigger_error(response.error_message); }
        return parseResponse(response);
    }

    using JsonCallback = std::function<void(Json)>;

    void postAsync(const QString& suffix, const Json& json, JsonCallback cb, const QString& contentType = "application/json") {
        const QString data = jsonToString(json);
        setParameters(suffix, data, contentType);
        session_.postAsync(contentType, [this, cb](Response response) {
            handleAsyncResponse(response, cb);
        });
    }

    void getAsync(const QString& suffix, JsonCallback cb, const QString& data = QString()) {
        setParameters(suffix, data);
        session_.getAsync([this, cb](Response response) {
            handleAsyncResponse(response, cb);
        });
    }

    void delAsync(const QString& suffix, JsonCallback cb) {
        setParameters(suffix, QString());
        session_.deleteAsync([this, cb](Response response) {
            handleAsyncResponse(response, cb);
        });
    }

    QString easyEscape(const QString& text) { return session_.easyEscape(text); }
    void debug() const { std::cout << token_.toStdString() << '\n'; }
    void setBaseUrl(const QString& url) { base_url = url; }
    QString getBaseUrl() const { return base_url; }

private:
    OpenAI(const QString& token,
           const QString& organization,
           bool throw_exception,
           const QString& api_base_url,
           const QString& beta,
           QNetworkAccessManager* network_manager)
        : session_{throw_exception, network_manager},
          token_{token},
          organization_{organization},
          throw_exception_{throw_exception}
    {
        if (token_.isEmpty()) {
            if (const char* env_p = std::getenv("OPENAI_API_KEY")) {
                token_ = QString::fromUtf8(env_p);
            }
        }
        if (api_base_url.isEmpty()) {
            if (const char* env_p = std::getenv("OPENAI_API_BASE")) {
                base_url = QString::fromUtf8(env_p) + "/";
            }
            else {
                base_url = "https://api.openai.com/v1/";
            }
        }
        else {
            base_url = api_base_url;
        }
        session_.setUrl(base_url);
        session_.setToken(token_, organization_);
        session_.setBeta(beta);
    }

    void setParameters(const QString& suffix, const QString& data, const QString& contentType = QString()) {
        const auto complete_url = base_url + suffix;
        session_.setUrl(complete_url);

        if (contentType != "multipart/form-data") {
            session_.setBody(data);
        }

        #if OPENAI_VERBOSE_OUTPUT
            std::cout << "<< request: " << complete_url.toStdString() << "  " << data.toStdString() << '\n';
        #endif
    }

    void checkResponse(const Json& json) {
        if (json.contains("error")) {
            const auto reason = jsonToString(json["error"].toObject());
            trigger_error(reason);

            #if OPENAI_VERBOSE_OUTPUT
                std::cerr << ">> response error :\n" << jsonToString(json, false).toStdString() << "\n";
            #endif
        }
    }

    bool isJson(const QString& data) {
        QJsonParseError error;
        QJsonDocument::fromJson(data.toUtf8(), &error);
        return error.error == QJsonParseError::NoError;
    }

    Json parseResponse(const Response& response) {
        Json json;
        if (isJson(response.text)) {
            json = stringToJsonObject(response.text);
            checkResponse(json);
        }
        else {
          #if OPENAI_VERBOSE_OUTPUT
            std::cerr << "Response is not a valid JSON";
            std::cout << "<< " << response.text.toStdString() << "\n";
          #endif
        }
        return json;
    }

    void handleAsyncResponse(const Response& response, JsonCallback cb) {
        if (response.is_error) {
            trigger_error(response.error_message);
        }
        cb(parseResponse(response));
    }

    void trigger_error(const QString& msg) {
        if (throw_exception_) {
            throw std::runtime_error(msg.toStdString());
        }
        else {
            std::cerr << "[OpenAI] error. Reason: " << msg.toStdString() << '\n';
        }
    }

public:
    CategoryModel           model     {*this};
    CategoryAssistants      assistant {*this};
    CategoryThreads         thread    {*this};
    CategoryCompletion      completion{*this};
    CategoryEdit            edit      {*this};
    CategoryImage           image     {*this};
    CategoryEmbedding       embedding {*this};
    CategoryFile            file      {*this};
    CategoryFineTune        fine_tune {*this};
    CategoryModeration      moderation{*this};
    CategoryChat            chat      {*this};
    CategoryAudio           audio     {*this};

private:
    Session                 session_;
    QString                 base_url;
    QString                 token_;
    QString                 organization_;
    bool                    throw_exception_;
};

inline QString bool_to_string(const bool b) {
    return b ? QStringLiteral("true") : QStringLiteral("false");
}

inline OpenAI& start(const QString& token = QString(), const QString& organization = QString(), bool throw_exception = true, const QString& api_base_url = QString())  {
    static OpenAI instance = OpenAI::create(token, organization, throw_exception, api_base_url);
    return instance;
}

inline OpenAI& instance() {
    return start();
}

inline Json post(const QString& suffix, const Json& json) {
    return instance().post(suffix, json);
}

inline Json get(const QString& suffix) {
    return instance().get(suffix);
}

inline CategoryModel& model() {
    return instance().model;
}

inline CategoryAssistants& assistant() {
    return instance().assistant;
}

inline CategoryThreads& thread() {
    return instance().thread;
}

inline CategoryCompletion& completion() {
    return instance().completion;
}

inline CategoryChat& chat() {
    return instance().chat;
}

inline CategoryAudio& audio() {
    return instance().audio;
}

inline CategoryEdit& edit() {
    return instance().edit;
}

inline CategoryImage& image() {
    return instance().image;
}

inline CategoryEmbedding& embedding() {
    return instance().embedding;
}

inline CategoryFile& file() {
    return instance().file;
}

inline CategoryFineTune& fineTune() {
    return instance().fine_tune;
}

inline CategoryModeration& moderation() {
    return instance().moderation;
}

inline Json CategoryModel::list() {
    return openai_.get("models");
}

inline Json CategoryModel::retrieve(const QString& model) {
    return openai_.get("models/" + model);
}

inline Json CategoryAssistants::create(Json input) {
    return openai_.post("assistants", input);
}

inline Json CategoryAssistants::retrieve(const QString& assistants) {
    return openai_.get("assistants/" + assistants);
}

inline Json CategoryAssistants::modify(const QString& assistants, Json input) {
    return openai_.post("assistants/" + assistants, input);
}

inline Json CategoryAssistants::del(const QString& assistants) {
    return openai_.del("assistants/" + assistants);
}

inline Json CategoryAssistants::list() {
    return openai_.get("assistants");
}

inline Json CategoryAssistants::createFile(const QString& assistants, Json input) {
    return openai_.post("assistants/" + assistants + "/files", input);
}

inline Json CategoryAssistants::retrieveFile(const QString& assistants, const QString& files) {
    return openai_.get("assistants/" + assistants + "/files/" + files);
}

inline Json CategoryAssistants::delFile(const QString& assistants, const QString& files) {
    return openai_.del("assistants/" + assistants + "/files/" + files);
}

inline Json CategoryAssistants::listFile(const QString& assistants) {
    return openai_.get("assistants/" + assistants + "/files");
}

inline Json CategoryThreads::create() {
    Json input;
    return openai_.post("threads", input);
}

inline Json CategoryThreads::retrieve(const QString& threads) {
    return openai_.get("threads/" + threads);
}

inline Json CategoryThreads::modify(const QString& threads, Json input) {
    return openai_.post("threads/" + threads, input);
}

inline Json CategoryThreads::del(const QString& threads) {
    return openai_.del("threads/" + threads);
}

inline Json CategoryThreads::createMessage(const QString& threads, Json input) {
    return openai_.post("threads/" + threads + "/messages", input);
}

inline Json CategoryThreads::retrieveMessage(const QString& threads, const QString& messages) {
    return openai_.get("threads/" + threads + "/messages/" + messages);
}

inline Json CategoryThreads::modifyMessage(const QString& threads, const QString& messages, Json input) {
    return openai_.post("threads/" + threads + "/messages/" + messages, input);
}

inline Json CategoryThreads::listMessage(const QString& threads) {
    return openai_.get("threads/" + threads + "/messages");
}

inline Json CategoryThreads::retrieveMessageFile(const QString& threads, const QString& messages, const QString& files) {
    return openai_.get("threads/" + threads + "/messages/" + messages + "/files/" + files);
}

inline Json CategoryThreads::listMessageFile(const QString& threads, const QString& messages) {
    return openai_.get("threads/" + threads + "/messages/" + messages + "/files");
}

inline Json CategoryThreads::createRun(const QString& threads, Json input) {
    return openai_.post("threads/" + threads + "/runs", input);
}

inline Json CategoryThreads::retrieveRun(const QString& threads, const QString& runs) {
    return openai_.get("threads/" + threads + "/runs/" + runs);
}

inline Json CategoryThreads::modifyRun(const QString& threads, const QString& runs, Json input) {
    return openai_.post("threads/" + threads + "/runs/" + runs, input);
}

inline Json CategoryThreads::listRun(const QString& threads) {
    return openai_.get("threads/" + threads + "/runs");
}

inline Json CategoryThreads::submitToolOutputsToRun(const QString& threads, const QString& runs, Json input) {
    return openai_.post("threads/" + threads + "/runs/" + runs + "/submit_tool_outputs", input);
}

inline Json CategoryThreads::cancelRun(const QString& threads, const QString& runs) {
    Json input;
    return openai_.post("threads/" + threads + "/runs/" + runs + "/cancel", input);
}

inline Json CategoryThreads::createThreadAndRun(Json input) {
    return openai_.post("threads/runs", input);
}

inline Json CategoryThreads::retrieveRunStep(const QString& threads, const QString& runs, const QString& steps) {
    return openai_.get("threads/" + threads + "/runs/" + runs + "/steps/" + steps);
}

inline Json CategoryThreads::listRunStep(const QString& threads, const QString& runs) {
    return openai_.get("threads/" + threads + "/runs/" + runs + "/steps");
}

inline Json CategoryCompletion::create(Json input) {
    return openai_.post("completions", input);
}

inline Json CategoryChat::create(Json input) {
    return openai_.post("chat/completions", input);
}

inline Json CategoryAudio::transcribe(Json input) {
    auto lambda = [input]() -> QMap<QString, QString> {
        QMap<QString, QString> temp;
        temp.insert("model", input["model"].toString());
        if (input.contains("language")) temp.insert("language", input["language"].toString());
        if (input.contains("prompt")) temp.insert("prompt", input["prompt"].toString());
        if (input.contains("response_format")) temp.insert("response_format", input["response_format"].toString());
        if (input.contains("temperature")) temp.insert("temperature", QString::number(input["temperature"].toDouble()));
        return temp;
    };
    openai_.setMultiformPart({"file", input["file"].toString()}, lambda());
    return openai_.post("audio/transcriptions", QString{}, "multipart/form-data");
}

inline Json CategoryAudio::translate(Json input) {
    auto lambda = [input]() -> QMap<QString, QString> {
        QMap<QString, QString> temp;
        temp.insert("model", input["model"].toString());
        if (input.contains("language")) temp.insert("language", input["language"].toString());
        if (input.contains("prompt")) temp.insert("prompt", input["prompt"].toString());
        if (input.contains("response_format")) temp.insert("response_format", input["response_format"].toString());
        if (input.contains("temperature")) temp.insert("temperature", QString::number(input["temperature"].toDouble()));
        return temp;
    };
    openai_.setMultiformPart({"file", input["file"].toString()}, lambda());
    return openai_.post("audio/translations", QString{}, "multipart/form-data");
}

inline Json CategoryEdit::create(Json input) {
    return openai_.post("edits", input);
}

inline Json CategoryImage::create(Json input) {
    return openai_.post("images/generations", input);
}

inline Json CategoryImage::edit(Json input) {
    QString prompt = input["prompt"].toString();
    QString mask = "";
    int n = 1;
    QString size = "1024x1024";
    QString response_format = "url";
    QString user = "";

    if (input.contains("mask")) mask = input["mask"].toString();
    if (input.contains("n")) n = input["n"].toInt();
    if (input.contains("size")) size = input["size"].toString();
    if (input.contains("response_format")) response_format = input["response_format"].toString();
    if (input.contains("user")) user = input["user"].toString();

    openai_.setMultiformPart({"image", input["image"].toString()},
        QMap<QString, QString>{
            {"prompt", prompt},
            {"mask", mask},
            {"n", QString::number(n)},
            {"size", size},
            {"response_format", response_format},
            {"user", user}
        }
    );
    return openai_.post("images/edits", QString{}, "multipart/form-data");
}

inline Json CategoryImage::variation(Json input) {
    int n = 1;
    QString size = "1024x1024";
    QString response_format = "url";
    QString user = "";

    if (input.contains("n")) n = input["n"].toInt();
    if (input.contains("size")) size = input["size"].toString();
    if (input.contains("response_format")) response_format = input["response_format"].toString();
    if (input.contains("user")) user = input["user"].toString();

    openai_.setMultiformPart({"image", input["image"].toString()},
        QMap<QString, QString>{
            {"n", QString::number(n)},
            {"size", size},
            {"response_format", response_format},
            {"user", user}
        }
    );
    return openai_.post("images/variations", QString{}, "multipart/form-data");
}

inline Json CategoryEmbedding::create(Json input) {
    return openai_.post("embeddings", input);
}

inline Json CategoryFile::list() {
    return openai_.get("files");
}

inline Json CategoryFile::upload(Json input) {
    openai_.setMultiformPart({"file", input["file"].toString()},
        QMap<QString, QString>{{"purpose", input["purpose"].toString()}}
    );
    return openai_.post("files", QString{}, "multipart/form-data");
}

inline Json CategoryFile::del(const QString& file_id) {
    return openai_.del("files/" + file_id);
}

inline Json CategoryFile::retrieve(const QString& file_id) {
    return openai_.get("files/" + file_id);
}

inline Json CategoryFile::content(const QString& file_id) {
    return openai_.get("files/" + file_id + "/content");
}

inline Json CategoryFineTune::create(Json input) {
    return openai_.post("fine-tunes", input);
}

inline Json CategoryFineTune::list() {
    return openai_.get("fine-tunes");
}

inline Json CategoryFineTune::retrieve(const QString& fine_tune_id) {
    return openai_.get("fine-tunes/" + fine_tune_id);
}

inline Json CategoryFineTune::content(const QString& fine_tune_id) {
    return openai_.get("fine-tunes/" + fine_tune_id + "/content");
}

inline Json CategoryFineTune::cancel(const QString& fine_tune_id) {
    return openai_.post("fine-tunes/" + fine_tune_id + "/cancel", Json{});
}

inline Json CategoryFineTune::events(const QString& fine_tune_id) {
    return openai_.get("fine-tunes/" + fine_tune_id + "/events");
}

inline Json CategoryFineTune::del(const QString& model) {
    return openai_.del("models/" + model);
}

inline Json CategoryModeration::create(Json input) {
    return openai_.post("moderations", input);
}

} // namespace _detail

using _detail::OpenAI;
using _detail::start;
using _detail::instance;
using _detail::post;
using _detail::get;
using _detail::model;
using _detail::assistant;
using _detail::thread;
using _detail::completion;
using _detail::edit;
using _detail::image;
using _detail::embedding;
using _detail::file;
using _detail::fineTune;
using _detail::moderation;
using _detail::chat;
using _detail::audio;
using _detail::Json;

} // namespace openai

#endif // OPENAI_HPP_
