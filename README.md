# CS301 WatchCat Project

## 1. Setup

### 1.1. Linter

We use `pre-commit` and `clang-format` to maintain code style.

Use `pip install pre-commit` to install `pre-commit` in your Python environment.

Run `pre-commit install` in the project root directory to initialize the hook. This prepares you an independent `clang-format` to use.

### 1.2. LCD driver version control

Currently, we switch between LCD drivers for v3-board and v4-board by configuration header.

Under `Core/Inc/display`, please create a file `lcd_drv_conf.h` using the following template. You must uncomment one of the two lines to define the version macro.
```c
#ifndef __LCD_DRV_CONF_H__
#define __LCD_DRV_CONF_H__

/* You must define one of the following macro according to the board you use. */

// #define LCD_DRIVER_V3
// #define LCD_DRIVER_V4

#endif /* __LCD_DRV_CONF_H__ */

```

Now you are free to build the STM32 project.

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
