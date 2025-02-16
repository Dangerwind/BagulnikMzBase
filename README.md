
## Центральная станция обработки и передачи информации системы "Багульник-М МЗ".

**Целью проекта:** создение интернет-устройства обеспечивющего обмен данными по интерфейсу RS-485 протоколу Багульник-М.

**Аппаратная часть:** микроконтроллер ESP32S3, интерфейс Ethernet W5500, дисплее OLED128x63 I2C, flash-память W25Q256.

Программа написана на **C++**. Используется **PlatformIo**, **Arduino**, **FreeRTOS**. Используется многозадачность, все фунцуии выполняются параллельно.

После включения устройства, программа подключается по WiFi или LAN к сети интернет и формирует Telegram-бот или Web-сервер.  
Все настриойки сетевых и прочих параметров осуществляются с испольованием дисплея и физических кнопок. Все параметры сохраняются в энергонезависимой памяти и не требуют повторной настройки после отключения питания.  
Устройство непрерывано работает по интерфейсу RS-485 обмениваясь пакетами с подключенными датчиками по протоколу "Багульник-М". 
После включения и настройки, пользователь может подключиться к Telegram-боту и просматривать полную информацию о состоянии всех датчиков плюключенных к устройству, просматривать архив в который сохраняютс все изменения состоянй датчиков и проводить дистационное тестирование. Telegram-бот будет присылать уведомления о критических изменениях в датчиках или потери связи с ними.
Если был выбран режим работы "Web-сервер", пользователь может открыть веб-браузер войти на стртаницу устройства (адрес и все параметры настраиваются заранее с использованием дисплея и кнопок на устройстве) на которой отображается вся информация о состоянй датчиков и скорости обмена. Так же можно просматривать архив сообщений и дистанционно тестировать работы устройства.

 **Логика работы** Программа разделена на несколько логических блоков.
1) Работа с дисплее и кнопками для настройки сетевых параметров, имен и паролей, режима работы устройства в сети (Telegram-бот или Web-сервер). Отображение скоростей обмена данных и меню для ручной настройки датчтков, удаление архивов и тд.
2) Опрос по RS-485 датчиков на линнии, формирование пакетов, расшифровка пакетов, выполенение скриптов, формирование информации для отображения пользователю.
3) Формирвоание Telegram-бота, реакция на запросы пользователя, вывод уведомления на основании информации полученной в пункте 2.
4) Формирвоание Web-сервера, вывод информации полученной в пункте 2 и тд.
5) Работа в flash-памятью, ведение архива, чтение данных и настроек.


