Задание на сокеты
ex1 - простые придложения точка-точка для демонстрации сетевого обмена
shema 1  -сервер + клиент/уд. админ по простой схеме, но со структурами данных и за чего она фактичекски и схема 2 в потенциале 
Клиент отсылает число серверу, тот его плюсует,спит и возвращает ответ. клиент бснова посылает число серверу , но у же то которое получил. В приложеии клиента создаются сразу несколько процессов. ну и можно несколько приложений запускать.

shema 2 - схема 2 - минимальные изменения схемы 1, клиент-администратор тот же
shema 3 - схема 3 - по идее должен был вытечь из схемы 3 но тут вышла осечка, в итоге не только
пришлось все подстраивать, но и то что хотел сделать не заработало, 
стер все и написал по проще с нуля. Здесь так же изменен клиент, и убрано засыпание на сервере. теперь клиент прото посылает число принимает ответ и завершается , но само приложение создает пачку клиентов, и как все они завершаться создает след пачку.  Это что бы протестировать именно переполнение очеререди и посмотреть скорость обработки.

shema 4 - мультиплексипрование еще не реализовал, будет в отдельном коммите.
broad_mult - сервера и клиенты для мультикаста и броадкаста

shema 3
А идея была следующая - не использовать механизмы очередей, ибо объекты уже хранят всю информацию 
- есть справочник серверов, и есть справочник Клиентов содержащий в себе его сокет, полследний по сути и является очередью. Примитивы синхронизации - это тоже потери в скорости.
И что бы выйти на максимальное быстродействие оставалось только решить проблему конфликтов,
когда несколько процессов пытаются лезть на один и тот же сокет. 
Хотел сделать так - пусть лезут, но пишут в структуре клиента поле, pid сервера  
который взялся за обработку, тогда через несколько машинных инструкций  проверим, 
остался ли тот пид, который присвоили мы, - тогда и обрабатываем. если нет, идти к следующему клиенту .

Возникли сложности в отладке в итоге упростил, скидываю в очередь сообщений индекс клиента из справочника, тогда кто первый получил тот и обрабатывает этот индекс.
