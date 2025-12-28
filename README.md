# Minimal Logger

> MIT License Copyright (c) 2025 kioz.wang

# Description

最通用、最迷你的日志框架。可用于 `C`, `C++`。

## Test and Run

```shell
$ rm -f *.gch *.o a.out && gcc -D__TEST_LOGGER__ *.[ch]
$ ./a.out 
[E]hello world
[W]hello world
[I]hello world
$ log2stderr=99 ./a.out 
[4fa25cf261][E]hello world
[E]hello world
[4fa25cf290][W]hello world
[W]hello world
[4fa25cf295][I]hello world
[I]hello world
[4fa25cf299][V]hello world
[4fa25cf29b][D]hello world
```

## Howto use

### Build

构建成独立的静态或动态库，或加入到代码树中一起构建。

### API

详见 [头文件](logger.h)。
