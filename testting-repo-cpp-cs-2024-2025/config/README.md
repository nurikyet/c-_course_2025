# Config

Хотим в любом месте кода в глобальном namespace объявлять следующую штуку:

```c++
Setting<std::string> first_param("first_param", "default_value", "help text", [](const std::string& value) {})
Setting<size_t> second_param("second_param", 10);
Setting<bool> third_param("third_param");
Setting<int32_t> fourth_param("fourth_param");
```

Параметры:

- Первый - имя параметра, обязательно должен быть
- Второй - значение по умолчанию, может отсутствовать
- Третий - текст для help, может отсутствовать
- Четвертый - handler который вызывается при изменении значения, может отсутствовать

Интерфейс следующий:
- ```template <typename T, typename ConfiguratorTag> Setting``` - КонфигуратортТэг - тэг для привязки к конкретному типу конфигуратора (```Configurator<ConfiguratorTag>```) (нужен только для тестов). по умолчанию выставлен в MainConfiguratorTag
- ```first_param.HasValue()``` - возвращает true если параметр установлен
- ```first_param.GetValue()``` - возвращает значение параметра, и кидает исключение ```BadSettingAccess``` если значчения нет

Еще должен быть класс Configurator (singleton). Интерфейс следующий:

- ```template <typename Tag> Configurator;``` - Tag - тэг для создания разных конфигураторов для тестов (по умолчанию MainConfiguratorTag)
- ```static ??? GetInstance()``` - выдает инстанс конфигуратора
- ```Configurator.SetValue(name, value)``` - устанавливает значение если параметр есть (иначе кидает исключение ```UnknownParametr```). Возвращает optional<T> - предыдущее значение и nullopt, если его не было
- ```Configurator.GetValue<T>(name)``` - возвращает значение если параметр есть (иначе кидает исключение ```UnknownParametr```), если значения установленно не было - nullopt. Если был указан неверный тип T - кидает исключение ```BadConfigValueType```
- ```Configurator.GetValueAsString(name)``` - возвращает значение в виде строки если параметр есть (иначе кидает исключение ```UnknownParametr```), если значения установленно не было - пустая строка. Проверятся будет только для типов с переопределенным оператором вывода << (используйте std::stringstream для перекладывания значений (на ревью про устройство stringstream спрашивать не будут))
- ```Configurator.GetHelp()``` - возвращает KeyValue хранилище, где по имени параметра хранится его help. Если help не установлен - пустая строка
- ```Configurator.GetHelp(name)``` - возвращает help параметра. Если help не установлен - пустая строка, если параметра нет - исключение
- ```Configurator.Init(std::map<string, string>)``` - инициализирует значения параметров. Будет инициализировать только параметры, для типа которых определен оператор ввода >> (используйте std::stringstream). Инициализация будет только корректными значениями в плане типа. Если у параметра есть дефолтное значение - перезатирает его
- ```Configurator.Init(file_name)``` - инициализирует значения параметров, читая .yaml файл с именем file_name. Будет инициализировать только параметры, для типа которых определен оператор ввода >> (используйте std::stringstream). Инициализация будет только корректными значениями в плане типа. Если у параметра есть дефолтное значение - перезатирает его
- ```Configurator.Drop(name)``` - сбрасывает значение параметра. Если параметра нет - кидает исключание
