#pragma GCC optimize("O3")

#include "FileServer.h"
#include <ESPmDNS.h>
#include "./tmpl_html.cpp"
#include "../../../manager/sd/SD_Manager.h"

namespace meow
{
    bool FileServer::_is_working = false;

    FileServer::~FileServer()
    {
        stop();
    }

    bool FileServer::begin(const char *server_dir, ServerMode mode)
    {
        if (_is_working)
            return false;

        _server_dir = server_dir;

        if (!_server_dir.equals("/") && _server_dir.endsWith("/"))
            _server_dir.remove(1, -1);

        if (_server_dir.isEmpty())
            _server_dir = "/";

        if (!_file_mngr.hasConnection())
            return false;

        if (!_file_mngr.dirExist(_server_dir.c_str()))
            return false;

        WiFi.softAP(_ssid, _pwd, 1, 0, 1);
        delay(10);

        if (_domain_name.isEmpty() || !MDNS.begin(_domain_name))
        {
            log_e("Помилка запуску mDNS");
            IPAddress IP = WiFi.softAPIP();
            _server_addr = "http://";
            _server_addr += IP.toString();
        }
        else
        {
            _server_addr = "http://";
            _server_addr += _domain_name;
            _server_addr += ".local";
        }

        log_i("AP addr: %s", _server_addr.c_str());

        _mode = mode;

        BaseType_t result = xTaskCreatePinnedToCore(startWebServer, "fileServerTask", (1024 / 2) * 20, this, 10, NULL, 1);

        if (result == pdPASS)
        {
            _is_working = true;

            log_i("File server is working now");

            if (_mode == SERVER_MODE_RECEIVE)
                log_i("mode == SERVER_MODE_RECEIVE");
            else
                log_i("mode == SERVER_MODE_SEND");

            return true;
        }
        else
        {
            log_e("fileServerTask was not running");
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
            return false;
        }
    }

    void FileServer::stop()
    {
        if (!_is_working)
            return;

        _must_work = false;

        _server->close();

        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);

        delete _server;
    }

    void FileServer::setDomainName(const char *domain_name)
    {
        char ch = domain_name[0];
        for (uint32_t i = 1; ch != '\0'; ++i)
        {
            if (ch == ' ' || ch == '/') // Очікується адекватне використання
            {
                log_e("Некоректний домен: [ %s ]", domain_name);
                return;
            }

            ch = domain_name[i];
        }

        _domain_name = domain_name;
    }

    String FileServer::getAddress() const
    {
        if (_is_working)
            return _server_addr;
        else
            return "";
    }

    void FileServer::fileServerTask(void *params)
    {
        _server = new WebServer(80);

        if (_mode == SERVER_MODE_RECEIVE)
        {
            _server->on("/", HTTP_GET, [this]()
                        { this->handleReceive(); });
            _server->on("/upload", HTTP_POST, []() {}, [this]()
                        { this->handleFile(); });
        }
        else
        {
            _server->on("/", HTTP_GET, [this]()
                        { this->handleSend(); });
        }

        _server->onNotFound([this]()
                            { this->handle404(); });

        _must_work = true;

        _server->begin();

        while (_must_work)
        {
            _server->handleClient();
            vTaskDelay(1);
        }

        _is_working = false;
    }

    void FileServer::handleReceive()
    {
        String html = HEAD_HTML;
        html += RECEIVE_TITLE_STR;
        html += RECEIVE_FILE_HTML;
        _server->sendHeader("Cache-Control", "no-cache");
        _server->send(200, "text/html", html);
    }

    void FileServer::handleSend()
    {
        // Відкрити директорію з файлами на SD
        File root = SD.open(_server_dir.c_str());

        if (!root)
        {
            log_e("Помилка відкриття директорії %s", _server_dir.c_str());
            _server->send(500, "text/html", "");
            return;
        }
        else if (_server->args() > 0)
        {
            String path = _server_dir;
            path += "/";
            path += _server->arg(0);

            if (!_file_mngr.fileExist(path.c_str(), true))
                handle404();
            else
            {
                File file = SD.open(path);

                _server->sendHeader("Content-Type", "application/force-download");
                _server->sendHeader("Content-Disposition", "attachment; filename=\"" + _server->arg(0) + "\"");
                _server->sendHeader("Content-Transfer-Encoding", "binary");
                _server->sendHeader("Cache-Control", "no-cache");
                _server->streamFile(file, "application/octet-stream");
                file.close();
            }
        }
        // Якщо відсутні параметри, відобразити список файлів в директорії
        else
        {
            String html = HEAD_HTML;
            html += SEND_TITLE_STR; // Заголовок
            html += MID_HTML;

            File file;
            while ((file = root.openNextFile()))
            {
                if (!file.isDirectory())
                {
                    html += HREF_HTML;
                    html += file.name();
                    html += "\">";
                    html += file.name();
                    html += "</a>";
                }
                file.close();
                vTaskDelay(1);
            }
            html += FOOT_HTML;
            _server->sendHeader("Cache-Control", "no-cache");
            _server->send(200, "text/html", html);
        }
    }

    void FileServer::handleFile()
    {
        static File input_file;

        HTTPUpload &uploadfile = _server->upload();
        _server->setContentLength(CONTENT_LENGTH_UNKNOWN);

        if (uploadfile.status == UPLOAD_FILE_START)
        {
            String file_name = _server_dir;
            file_name += "/";
            file_name += uploadfile.filename;

            log_i("Запит на створення файлу %s", file_name.c_str());

            input_file = SD.open(file_name.c_str(), FILE_READ);

            bool file_open_fail = false;

            if (input_file)
            {
                bool is_dir = input_file.isDirectory();
                input_file.close();

                if (is_dir)
                    file_open_fail = true;
                else
                    SD.remove(file_name.c_str());
            }

            if (!file_open_fail)
            {
                input_file = SD.open(file_name.c_str(), FILE_APPEND, true);

                if (!input_file)
                    file_open_fail = true;
                else
                    log_i("Файл створено");
            }

            if (file_open_fail)
            {
                log_e("Не можу відкрити файл %s на запис", file_name.c_str());
                _server->send(500, "text/html", "");
            }
        }
        else if (uploadfile.status == UPLOAD_FILE_WRITE)
        {
            if (input_file)
            {
                input_file.write(uploadfile.buf, uploadfile.currentSize);
                vTaskDelay(1);
            }
        }
        else if (uploadfile.status == UPLOAD_FILE_END || uploadfile.status == UPLOAD_FILE_ABORTED)
        {
            if (input_file)
            {
                input_file.close();

                handleReceive();

                if (uploadfile.status == UPLOAD_FILE_END)
                    log_i("Файл отримано");
                else
                    log_i("Завантаження перервано");
            }
            else
            {
                log_e("Необроблений файл");
                _server->send(500, "text/html", "");
            }
        }
    }

    void FileServer::handle404()
    {
        String html = HEAD_HTML;
        html += NOT_FOUND_TITLE_STR;
        html += MID_HTML;
        html += NOT_FOUND_BODY_START;
        html += _server_addr;
        html += NOT_FOUND_BODY_END;
        html += FOOT_HTML;
        _server->send(404, "text/html", html);
    }

    void FileServer::startWebServer(void *params)
    {
        FileServer *instance = static_cast<FileServer *>(params);
        instance->fileServerTask(params);
        log_i("FileServer task finished");
        vTaskDelete(NULL);
    }
}