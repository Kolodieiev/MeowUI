# Контекст
Уся логіка прошивки розміщується в спеціальних класах, які повинні наслідуватися від _IContext_. Ці класи називаються контекстом. Контекст можна уявляти окремим повноцінним модулем прошивки. Для розробника кожен такий модуль це _main.cpp_ на максималках. Кожен контекст має свій власний набір змінних, свій setup, loop, а також update, в якому відбувається обробка вводу та формування кадру для виводу на дисплей. В кожен момент часу активним може бути тільки один модуль. Якщо потрібно виконувати паралельно декілька завдань, рекомендується використання задач FreeRTOS зі зворотніми викликами. Також можливм є використання псевдопаралелізму з мітками часу.

###  Як це працює
Коли починає виконуватися програма прошивки, створюється _задача менеджера контекстів_. Його єдина функція це контроль та перемикання контекстів першого рівня. Модулі одного рівня не повинні знати про існування один одного, все що їм потрібно для перемикання це числовий ідентифікатор наступного контексту. Теоретично на одному рівні може одночасно знаходитися до 255 контекстів. Проте додавати всі модулі на один рівень не рекомендується. Краще за все буде об’єднання їх в умовні логічні групи. Це досягається за рахунок того, що кожен модуль може сам виступати менеджером контекстів. Проте тільки розробник вирішує на якому рівні повинен знаходитися кожен із модулів.  

Розглянемо простий приклад. Маємо _контекст домашнього екрану_, _контекст головного меню_ та _налаштувань_. Усі вони знаходяться на першому рівні й керуються менеджером контекстів. Кожен доступний тік, який було виділено UI-задачі, менеджер контекстів викликає тік у поточного активного модуля. Інших модулів на цьому ж рівні не існує фізично в цей момент.  

Тепер, якщо потрібно додати декілька вікон в _налаштування_, наприклад, вікно з налаштуванням годинника, вікно з налаштуванням точки доступу, тощо, можна обрати один зі шляхів:  

* Додати перемикання шаблонів GUI та їх стану в середину контексту налаштувань (не рекомендується через заплутування логіки).
* Додати окремо контексти для кожної вкладки налаштувань, та помістити їх на перший рівень глибини (не бажано).
* Створити окремо контексти для кожної вкладки, та організувати їх перемикання в межах контексту головного вікна налаштувань.

У третьому випадку відбуватиметься наступне:   

* _Менеджер контекстів_ викликає _tick_ у головного вікна модуля _налаштувань_.  
* Поки користувач не натиснув вибір якогось певного пункту налаштувань, головний модуль _налаштувань_ утримує контекст, оброблює натискання і тд.  
* Якщо користувач обирає певну вкладку налаштувань зі списку, головний _модуль налаштувань_ повинен створити відповідний вкладений контекст і _передати_ йому керування.  
* Тепер _менеджер контекстів_ продовжує викликати _tick_ у основного модуля налаштувань, а той віддає увесь свій час _одному із вкладених активних контекстів_, поки той не буде звільнено. 

Завдяки такому механізму може досягатися “нескінченна” вкладеність глибини контексту. 

!!! note "Важливо" 
    Якщо у вас є модуль де використовується метод loop, тоді краще розмістити його на першому рівні. Це позбавить необхідності передавати виклик loop в глибину. 


Зміна контексту відбувається дуже просто. Під час виклику спеціального методу, до нього передається числовий ідентифікатор модуля, який повинен бути створений наступним. Після цього поточний контекст підніме прапор, який означатиме, що його потрібно звільнити. Якщо контекст не відкриває інші модулі, достатньо просто підняти прапор, який сигналізує про завершення роботи поточного контексту. Далі все відбувається, як описано вище. Об’єкт, який керує поточним контекстом, прочитає цей прапор, видалить активний модуль і виконає інші дії в залежності від логіки програми.
