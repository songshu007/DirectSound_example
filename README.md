# DirectSound_example
DirectSound play WAVE example

这是一个使用 DirectSound 的最小示例

我自己实现了一个读取 WAVE 文件的类，我发现网上的代码大多数都是直接读取 WAVE 头数据直接填充到固定的结构体的，而我发现 fmt 的字段长度并非都是一样的，用一个固定的格式读取可以变化的格式必然会出错，比如格式工厂就会在 fmt 字段夹带一些私货
