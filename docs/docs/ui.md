# Макети UI
Контекст _може_ містити шаблон, що складається з віджетів, і який буде виводитися на дисплей кожен кадр. 

**Віджет** це найменший компонент графічного інтерфейсу користувача, що має стандартні зовнішній вигляд та поведінку.  

Мати шаблон UI в модулі не обов'язково. Наприклад, якщо пристрій використовується у вигляді пульта дистанційного керування, в якому потрібно обробляти тільки натискання кнопок і надсилати певний сигнал. Проте такий варіант не буде  розглядатися детально.  

Шаблоном може виступати будь який об’єкт з типом, що успадкований від _IWidgetContainer_. Як можна зрозуміти з імені, _IWidgetContainer_ це абстрактний клас, що являється контейнером для віджетів.  

!!! tip "Підказка"
    _IWidgetContainer_ є успадкованим від класу _IWidget_, тому також може бути вкладеним до _IWidgetContainer_. 

На даний момент в Meowui реалізовано наступні віджети:  

* **Label** - використовується для виводу тексту.
* **TextBox** - успадкований від Label, але має дещо іншу поведінку, та слугує полем для вводу тексту або паролю.
* **Menu**. Існує два види меню - _фіксоване_ та _динамічне_. Обидва виконують однакову роль з виведенням списків, але мають принципові відмінності у функціонуванні. _Фіксоване(FixedMenu)_ формується відразу повністю у пам'яті. _Динамічне(DynamicMenu)_ - створюється частинами, в процесі прокручування списку таким чином, щоб мінімізувати використання оперативної пам'яті мікроконтролера. Кожна наступна порція даних замінює попередню. Контроль завантаження даних повинен організувати розробник. Динамічне меню  пропонує тільки інтерфейс для їх зручного додавання та гарантує виклик обробників прокрутки списку.
* **MenuItem** - елемент списку меню.
* **Scrollbar** - панель, що візуально показує прогрес прокручування списку.
* **Progressbar** - віджет, який може відображати прогрес виконання будь-якої дії.
* **Spinbox** - елемент, який дозволяє зациклювати діапазон чисел.
* **Toggle** - віджет для відображення стану, який може бути описаний як “увімкнено/вимкнено”. 
* **Keyboard** та **KeyboardRow** для формування розкладок клавіатури.
* **Image** - для формування і виводу зображень на дисплей із масивів, що містять інформацію про пікселі зображення.
* **EmptyLayout** - віджет, який є простою панеллю для компоновки інших віджетів.

### Як це працює
Кожен віджет сам контролює свою відрисовку на дисплеї відповідно до заданих параметрів. Все, що вимагається від розробника, це налаштувати параметри кожного віджета і додати його до контейнера в момент створення. Віджети можна додавати, видаляти та змінювати не тільки в процесі формування шаблону, але й на льоту через вказівники, в залежності від логіки програми.

Усі віджети спроектовані таким чином, щоб мінімізувати навантаження на ядро мікроконтролера під час їх відображення на дисплей. Тому, якщо віджет не було змінено, він автоматично  пропускатиме свою чергу малювання. Хоча його можна примусово перемалювати. 
Також є важливим той факт, що всі віджети відрисовуються в тому порядку, в якому вони були додані до свого контейнера. 
Про ці особливості потрібно пам'ятати, коли використовується багатошарова компоновка макету.  

### Позиціонування
Позиціонування віджетів відбувається відносно лівого верхнього кута свого батьківського контейнера або дисплею, якщо батьківський контейнер відсутній. Батьківський контейнер присвоюється кожному віджету автоматично, коли той додається до контейнера. Таким чином, якщо у батьківського контейнера задано координати позиції {5,5}, а у вкладеного {0,0} (по замовчуванню), то вони обоє відмалюються з точки {5,5}. Якщо у вкладений контейнер додати віджет з координатами {5, 10} то його місце на дисплеї буде в точці {10, 15}. Тому що {5+0+5, 5+0+10}. Вигода від відносного позиціонування полягає в тому, що під час розробки не потрібно пам'ятати і вручну розраховувати позицію кожного нового віджета, що додається до контейнера.


### Вивід на дисплей
Існує два варіанти виводу макета на дисплей:  
1. Малювання кожного віджета _по черзі на дисплей_.  
2. Малювання кожного віджета по черзі в буфер дисплею. І тільки коли всі віджети будуть нарисовані, малювання _усього буферу на дисплей_ за одну транзакцію.  

Перший варіант підійде у тому випадку, якщо на платі відсутня велика кількість оперативної пам'яті. Під час прямого малювання віджета на дисплей, використовується зовсім незначний об'єм RAM. Але в процесі малювання кожен віджет буде окремо надсилати своє зображення по шині SPI на дисплей, що, по-перше, значно сповільнить процес малювання кожного кадру, по-друге, під час інтенсивного перемальовування зображення на дисплеї можуть виникати графічні артефакти.  
У другому випадку вивід зображення на дисплей відбуватиметься набагато швидше та плавніше. Проте для буферу буде зарезервовано w\*h\*b\*2. Де w та h це розмір дисплея, а b - кількість байт для кодування кольору пікселів.  
Обрати тип виводу зображення можна в налаштуваннях Meowui.  

### Асинхронність
Система шаблонів Meowui частково підтримує асинхронність. Наприклад, можна додавати, видаляти чи виконувати пошук віджетів у контейнері зі зворотніх викликів інших задач. Проте розробник прошивки мусить сам контролювати, щоб зворотній виклик не звернувся через вказівник до віджета, який вже було видалено. Це ціна можливості кешування вказівників на віджети. Якщо в контексті є обробка зворотніх викликів, то також _обов'язково_ має бути система контролю станів(режимів) контексту. Найпростіше організувати таку систему на перечисленні, що містить всі можливі стани контексту, та змінній, в яку записується стан.  
Контекст НЕ повинен розпочинати своє звільнення, якщо він запустив іншу задачу та встановив прослуховування подій цієї задачі. Перед виходом, перш за все, необхідно відписатися від обробки зворотніх викликів, та, якщо є можливість, зупинити запущену задачу. Лише після цього можна піднімати прапор звільнення контексту.