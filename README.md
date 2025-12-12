# tg-cli

`tg-cli` — консольный клиент Telegram, который использует TDLib для получения чатов, отправки сообщений и просмотра истории. Проект написан на C++17 и собирается через CMake.

## Требования
- CMake 3.16+ и компилятор с поддержкой C++17 (g++/clang++)
- Git submodules/архивы TDLib уже лежат в `third_party/td`
- OpenSSL и zlib (для сборки TDLib)

## Настройка окружения
1. Получите `api_id` и `api_hash` в [my.telegram.org](https://my.telegram.org/apps).
2. Создайте файл `.env` в корне репозитория со значениями (без кавычек):
   ```env
   API_ID=<ваш_api_id>
   API_HASH=<ваш_api_hash>
   ```
   CMake автоматически подхватит эти переменные и подставит их в параметры TDLib.

## Сборка TDLib
Скрипт `build.sh` собирает TDLib в `third_party/td/tdlib` (путь совпадает с дефолтным `TDLIB_ROOT`, который ожидает CMakeLists):
```bash
./build.sh
```
Скрипт создаёт отдельный `build` в `third_party/td`, компилирует TDLib и устанавливает её артефакты в `third_party/td/tdlib`.

## Сборка проекта
Стандартная out-of-source сборка:
```bash
mkdir -p build
cd build
cmake ..               # при необходимости можно указать -DTDLIB_ROOT=<путь_к_tdlib>
cmake --build .        # соберёт tg-cli, tg-cli-lib и тестовые цели
```
Исполняемый файл `tg-cli` появится в каталоге `build`.

## Запуск
Перед выполнением команд убедитесь, что TDLib и API ключи настроены. Бинарь принимает следующие команды (см. `TgClientFacade`):

- `auth-status` — показать текущий статус авторизации.
- `login-phone <phone>` — отправить номер телефона.
- `login-code <code>` — подтвердить код из Telegram.
- `logout` — выйти из аккаунта.
- `chats [limit]` — вывести список чатов (по умолчанию 20).
- `search-chats <query>` — поиск чатов по строке.
- `chat-info <chat_id>` — информация о конкретном чате.
- `history <chat_id> [limit]` — история сообщений в чате.
- `set-target <chat_id>` — сохранить чат как «целевой» для последующего просмотра.
- `get-target-history [limit]` — показать историю сохранённого чата.
- `send <chat_id> <message...>` — отправить текстовое сообщение.

Пример: вывод чатов и отправка сообщения в первый из них:
```bash
./tg-cli chats 10
./tg-cli send <chat_id> "Привет из tg-cli!"
```

## Тесты
После конфигурации сборки тесты можно запустить из каталога `build`:
```bash
ctest --output-on-failure
```