# CS301 WatchCat Project

## 1. Setup

### 1.1. Linter

We use `pre-commit` and `clang-format` to maintain code style.

Use `pip install pre-commit` to install `pre-commit` in your Python environment.

Run `pre-commit install` in the project root directory to initialize the hook. This prepares you an independent `clang-format` to use.

## 2. Commit

### 2.1. Before Commit

After you stage the changes and before you issue a commit:
1. Run `pre-commit run clang-format` to check the code style.
2. Files not conforming to the style will be changed (you will see a **FAIL**). Please check and stage these new changes.
3. Now you can issue the commit. Please adhere to the [message format](#22-message-format).

### 2.2. Message Format

When making commits, try to use [commitizen](https://commitizen.github.io/cz-cli/) or its other implementations (e.g., [vsc-commitizen](https://marketplace.visualstudio.com/items?itemName=KnisterPeter.vscode-commitizen) in VS Code).

Or, if you make commits manually, try to adhere to the format of `<TYPE>(<SCOPE>): <SHORT and IMPERATIVE summary, lower case, no period>`

`<TYPE>` is one of the following describing the nature of the commit:
![](images/commitizen-example.png)

`<SCOPE>` is optional but highly recommended for `feat`! Some scopes you would expect in this project are:
* `core`: core logic, HAL stuff, interrupts, peripherals, utilities, etc.
* `display`: LCD driver, *lvgl* library, etc.
* `home`: home screen
* `chat`: chat component
* `calc`: calculator component
* `album`: album component

Good examples are:
```
feat(display): optimize LCD flush
feat(calc): add calculator app
build: add define for lvgl
```

## 3. UI System and Build

The UI designer is powered by [*SquareLine Studio*](https://squareline.io/).

The UI project is located at `./SquareLine`. Please use *SquareLine Studio* to open the project through `watch_ui.spj`.

**Configure the export path in `File -> Project Settings`. Set both `Project Export Root` and `UI File Export Root` to the `SquareLine/export` directory. This is where the UI files are generated.**

Please make sure to click `Export -> Export UI Files` to export all UI files if:

* This is the first time you configure the project.
* UI has been updated inside `SquareLine Studio`.

The STM32 project build system will automatically import these files.

Implement callback functions and interaction with UIs in `Core/Src/port/sl_ui.c`
