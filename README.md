# ACGN收藏管理工具

一个基于Bangumi的ACGN收藏管理工具

## 主要功能
- Bangumi账号同步收藏
- 多站点视频搜索和播放
- 本地数据库缓存
- PikPak下载
- <details>
    <summary> <b>未来计划</b> </summary>
  
  * 视频本地缓存
  * 普通BT下载
  * 边下边看
  * 弹幕 
  * 超分辨率
  * 规则编辑器
  * 规则自定义
  * 规则排序
  * 记录播放历史
  * 剧时间表
  * 漫画.轻小说更新提醒
  * 漫画.轻小说源
  * GalGame下载.启动.统计
  * 多端适配
  </details>

## 安装使用
python 3.11+  

安装依赖：`pip install -r requirements.txt`

运行程序：`python main.py`

## 技术
- UI 使用 [Qt for Python](https://doc.qt.io/qtforpython-6/) pyside6
- 视频播放器 使用 [VLC](https://www.videolan.org/vlc)

## 参与开发
欢迎提交 PR 参与开发

## 致谢
感谢 [animeko](https://github.com/open-ani/animeko) 为本项目提供灵感来源  
感谢 [Kazumi](https://github.com/Predidit/Kazumi) 为本项目提供灵感来源  
感谢 [Bangumi](https://bangumi.tv) 开放API，提供番剧元数据  
感谢 [PikPak](https://mypikpak.com/zh-CN) 提供了高效的下载方式