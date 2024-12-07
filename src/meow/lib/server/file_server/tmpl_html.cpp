#include <Arduino.h>

const char HEAD_HTML[] = R"rawliteral(<!DOCTYPE html><html lang="uk"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>)rawliteral";

const char MID_HTML[] = "</title></head><body>";
const char FOOT_HTML[] = "</body></html>";

const char RECEIVE_TITLE_STR[] = "Надіслати файл";
const char SEND_TITLE_STR[] = "Список файлів";

const char NOT_FOUND_TITLE_STR[] = "404";
const char NOT_FOUND_BODY_START[] = "<h1>Файл не знайдено</h1><a href=\"";
const char NOT_FOUND_BODY_END[] = "\">На головну</a>";

const char RECEIVE_FILE_HTML[] = R"rawliteral(</title><style>body{font-family:Arial,sans-serif;display:flex;flex-direction:column;align-items:center;padding:20px;}h1{color:#333;}.form-container{display:flex;flex-direction:column;align-items:center;}.progress-bar{width:100%;height:20px;background-color:#e0e0e0;border-radius:5px;margin-top:10px;overflow:hidden;}.progress{height:100%;width:0%;background-color:#4caf50;border-radius:5px;transition:width 0.2s;}</style></head><body><h1>Надіслати файл</h1><div class="form-container"><form id="uploadForm"><input type="file" id="fileInput" required><button type="submit">Надіслати</button></form><div class="progress-bar" id="progressBar" style="display:none;"><div class="progress" id="progress"></div></div></div><script>document.getElementById('uploadForm').addEventListener('submit',function(event){event.preventDefault();const fileInput=document.getElementById('fileInput');if(!fileInput.files.length)return alert('Будь ласка, виберіть файл для завантаження');const file=fileInput.files[0];const formData=new FormData();formData.append('file',file);const xhr=new XMLHttpRequest();xhr.open('POST','/upload');xhr.upload.addEventListener('progress',function(event){if(event.lengthComputable){const percentComplete=(event.loaded/event.total)*100;const progressBar=document.getElementById('progress');progressBar.style.width=percentComplete+'%';progressBar.style.backgroundColor='#4caf50';}});xhr.addEventListener('loadstart',function(){document.getElementById('progressBar').style.display='block';});xhr.addEventListener('load',function(){alert('Файл успішно завантажено!');document.getElementById('progress').style.width='0%';document.getElementById('progressBar').style.display='none';fileInput.value='';});xhr.addEventListener('error',function(){const progressBar=document.getElementById('progress');progressBar.style.backgroundColor='#f44336';alert('Сталася помилка при завантаженні файлу');});xhr.send(formData);});</script></body></html>)rawliteral";

const char HREF_HTML[] = "<br><br><a href=\"/?file_name=";