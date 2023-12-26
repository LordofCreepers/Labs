# Лабораторные работы группы МОАИС-23/1 за 1-ый семестр
Как загрузить свой проект?
1. Создайте аккаунт на GitHub.
2. Скиньте мне (Зотов В.) ссылку на ваш аккаунт.
3. Примите приглашение.
* Подождите, пока я вам напишу
* Обновляйте страницу репозитория, пока не увидите синюю плашку
* Нажмите "View invitation"
* Нажмите "Accept invitation"
4. Поместите ваш проект в папку Labs/Фамилия_латиницей.
5. Загрузите папку Labs в репозиторий.

Чтобы загрузить файлы...

## Использование браузерной версии GitHub (Простой метод)
1. Нажмите на выпадающий список "Add file".
2. Выберите "Upload existing files".
3. Перетащите папку Labs на главную панель.
4. Опишите коммит в поле "Added files via upload". Можете оставить поле пустым
5. Убедитесь, что выбрана опция "Commit directly to the main branch"
6. Нажмите кнопку "Commit changes"

## Использование Git Bash (Продвинутый метод)
1. Скачайте и установите инструменты git (Windows: https://gitforwindows.org/).
2. Запустите Git Bash. На этом этапе можно пропустить следующий шаг, если вы запустите Bash, нажав правой кнопкой мыши на пустое место в папке, где лежит Labs, и выберите Git Bash here в контекстном меню.
3. Перейдите в папку, где лежит папка Labs с помощью команды [`cd`](https://ru.wikipedia.org/wiki/Cd_(команда)).
4. Впишите следующие команды одну за другой:
[```git init```](https://git-scm.com/docs/git-init)
[```git add Labs```](https://git-scm.com/docs/git-add)
[```git commit -m "Изменения, которые вы внесли в свою ветку. Можете написать всё, что угодно"```](https://git-scm.com/docs/git-commit)
[```git remote add origin https://github.com/LordofCreepers/Labs.git```](https://git-scm.com/docs/git-remote)
[```git push origin main```](https://git-scm.com/docs/git-push)